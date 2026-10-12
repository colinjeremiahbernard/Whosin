#pragma once
#include <crow.h>
#include <atomic>
#include "RateLimiter.h"
#include "../services/RoomService.h"

struct RequestGuard
{
    struct context {};
    RoomService *rooms = nullptr;
    RateLimiter limiter;
    std::atomic<unsigned long> requests{0}, errors{0};
    void before_handle(crow::request &request, crow::response &response, context &)
    {
        ++requests;
        if (request.url == "/health") return;
        const bool write = request.method == crow::HTTPMethod::POST;
        if (request.body.size() > 4096)
        {
            response = crow::response(413, R"({"error":"Request is too large"})");
            response.end();
            return;
        }
        // Do not trust caller-supplied forwarding headers as client identity.
        if (!limiter.allow(request.remote_ip_address, write, request.url == "/api/rooms" && write))
        {
            response = crow::response(429, R"({"error":"Too many requests. Please wait a minute."})");
            response.add_header("Retry-After", "60");
            response.end();
            return;
        }
        const std::string prefix = "/api/rooms/";
        if (!request.url.starts_with(prefix)) return;
        const auto rest = request.url.substr(prefix.size());
        const auto slash = rest.find('/');
        const auto code = rest.substr(0, slash);
        if (rest == code + "/join") return;
        std::string id = request.get_header_value("X-Player-Id");
        if (write)
        {
            const auto body = crow::json::load(request.body);
            if (body && body.has("playerId") && body["playerId"].t() == crow::json::type::String)
                id = body["playerId"].s();
        }
        if (!rooms || !rooms->authenticate(code, id, request.get_header_value("X-Session-Token")))
        {
            response = crow::response(403, R"({"error":"Your session ended. Rejoin the room."})");
            response.end();
        }
    }
    void after_handle(crow::request &, crow::response &response, context &)
    {
        if (response.code >= 400) ++errors;
        response.add_header("X-Content-Type-Options", "nosniff");
        response.add_header("X-Frame-Options", "DENY");
        response.add_header("Referrer-Policy", "no-referrer");
        response.add_header("Permissions-Policy", "camera=(), microphone=(), geolocation=()");
        response.add_header("Content-Security-Policy",
            "default-src 'self'; script-src 'self'; style-src 'self'; "
            "connect-src 'self' wss:; img-src 'self'; object-src 'none'; "
            "base-uri 'none'; frame-ancestors 'none'; form-action 'self'");
    }
};
using WhosinApp = crow::App<RequestGuard>;
