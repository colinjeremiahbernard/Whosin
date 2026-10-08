"use strict";

window.whosin = {
  message(text) {
    document.getElementById("message").textContent = text;
  },
  async api(path, body) {
    const controller = new AbortController();
    const timeout = setTimeout(() => controller.abort(), 10000);
    const options = { cache: "no-store", signal: controller.signal };
    if (body !== undefined) {
      options.method = "POST";
      options.headers = { "Content-Type": "application/json" };
      options.body = JSON.stringify(body);
    }
    try {
      const response = await fetch(path, options);
      const data = await response.json();
      if (!response.ok) {
        const error = new Error(data.error || "Something went wrong.");
        error.status = response.status;
        throw error;
      }
      return data;
    } finally {
      clearTimeout(timeout);
    }
  }
};
