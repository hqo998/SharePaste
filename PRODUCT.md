# Product

<!-- impeccable:product-schema 1 -->

## Platform

web

## Users

Self-hosters and developers who need an instant, zero-account pastebin to share code snippets, logs, configuration files, or plain-text notes via a short shareable link without account registration or heavy infrastructure.

## Product Purpose

SharePaste lets anyone paste text or code and immediately generate a shareable URL (`/<15-char-code>`). Success means a visitor can paste content, click Share, and copy a working link in seconds, while operators can deploy and maintain the entire service with a single lightweight container and persistent SQLite storage.

## Positioning

An ultra-lightweight (~7.5MB Docker image), self-hostable pastebin backed by a compiled C++ HTTP server (`cpp-httplib` + SQLite3) and a minimal React + Tailwind CSS v4 frontend—delivering near-zero resource overhead, built-in IP token-bucket rate limiting, and zero external runtime dependencies.

## Operating Context

- **Self-hosted & homelab deployments:** Run via Docker or Docker Compose behind optional reverse proxies (`SP_TrustedProxies`), configured entirely through `SP_*` environment variables, with `/status` for container health checks and `/data/sharepaste.db` for persistence.
- **Browser workflow:** Users land directly in the full-viewport monospace editor (`/`), type or paste content, toggle line wrapping (`Wrap`), click `Share` to POST to `/api/new`, click the generated link field to copy it to the clipboard, or open an existing `/<code>` URL to inspect the paste and its view counter (`👁`).

## Capabilities and Constraints

- **Core editor & sharing (`/*`):** Full-screen monospace text editor with dynamic line numbering (aware of soft wrapping), soft/hard line wrap toggle, `New` paste reset, `Share` action generating a 15-character alphanumeric ID, click-to-copy share URL input, view count display, `sessionStorage` paste caching, and planned Tab indentation support.
- **Instance metadata & governance (`/about`):** Displays service overview, Reporting & Terms of Service, and administrator contact email fetched dynamically from `/api/email` (`SP_AdminContactEmail`).
- **Backend & security constraints:**
  - Strict Content Security Policy (`default-src 'self'; style-src 'self' 'unsafe-inline'; script-src 'self'; object-src 'none';`) and security headers (`X-Frame-Options: DENY`, `X-Content-Type-Options: nosniff`, `Strict-Transport-Security`).
  - Self-contained asset serving (`/www` and `/assets` mounted directly by the C++ binary)—no third-party CDNs or external font/script requests at runtime.
  - Configurable maximum paste size (`SP_RateLimit_MaxPasteSize`, exposed via `/api/maxsize`) and IP-based token-bucket rate limiting (`SP_RateLimit_*`).
- **Intentional minimalism:** Plain-text and code snippet sharing without user accounts, auth flows, or bloat.

## Brand Commitments

- **Name:** SharePaste
- **Logo / Icon:** `/www/favicon.svg` (clipboard + document vector icon)
- **Live reference instance:** `https://26072004.xyz`
- **Repository:** `https://github.com/hqo998/SharePaste`

## Evidence on Hand

- Existing C++17/20 backend in `src/backend/` (`main.cpp`, `dbmanager`, `ratelimiter`, `utility`, `testtools`) and CMake/Docker build pipeline (`CMakeLists.txt`, `dockerfile`).
- Existing React 19 + TypeScript + Vite + Tailwind CSS v4 web client in `Web_Client/` (`src/page/Home.tsx`, `src/page/About.tsx`, `src/ui/header.tsx`, `src/ui/footer.tsx`).
- Brand and app icons in `Web_Client/public/www/` (`favicon.svg`, `favicon.ico`, `icons.svg`, PWA manifest icons).
- Do not fabricate customer testimonials, SLA/uptime guarantees (explicitly disclaimed in Terms of Service), or third-party integrations.

## Product Principles

1. **Editor First, Zero Ceremony:** The root route is the workspace itself—never put marketing interstitials, sign-up gates, or modal clutter between arrival and pasting.
2. **Self-Contained & Lightweight:** Every frontend decision must respect the tiny container footprint and strict self-hosted CSP (`'self'`), avoiding external runtime requests or heavy client bundles.
3. **Readable at a Glance:** Monospace legibility, accurate line numbers across wrap states, and clear action affordances (`New`, `Share`, copy link, `Wrap`, view count) take priority over decorative chrome.
4. **Operator Simplicity:** Instance-specific details (admin contact email, paste size limits, rate limits) come cleanly from backend environment configuration without requiring frontend rebuilds.
