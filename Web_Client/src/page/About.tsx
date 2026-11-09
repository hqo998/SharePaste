import { useState, useEffect } from "react";
import { NavLink } from "react-router-dom";
import Footer from "../ui/footer";
import Header from "../ui/header";

function About() {
  const [contactEmail, setContactEmail] = useState("");

  useEffect(() => {
    document.title = "About | SharePaste";

    const fetchContactEmail = async () => {
      try {
        const res = await fetch("/api/email");
        if (!res.ok) {
          return;
        }
        const data = (await res.text()).trim();

        if (!data || data.toLowerCase().startsWith("<!doctype html")) {
          return;
        }

        setContactEmail(data);
      } catch {
        // Standalone dev mode without C++ backend
      }
    };

    fetchContactEmail();
  }, []);

  return (
    <div className="min-h-dvh flex flex-col bg-[var(--bg-gutter)] text-[var(--text-primary)]">
      <Header />

      <main className="sp-scroller flex-1 w-full px-5 sm:px-8 py-12 sm:py-16">
        <div className="w-full max-w-[68ch] mx-auto flex flex-col items-center">
          <NavLink
            to="/"
            title="Return to SharePaste editor"
            className="mb-5 block rounded-xl transition-opacity duration-150 hover:opacity-90"
          >
            <img
              src="/www/favicon.svg"
              alt="SharePaste logo"
              width={112}
              height={112}
              className="w-24 h-24 sm:w-28 sm:h-28 object-contain block"
            />
          </NavLink>

          <h1 className="text-3xl sm:text-5xl font-extrabold text-[var(--text-primary)] tracking-tight text-center m-0">
            SharePaste
          </h1>

          <p className="mt-3 mb-6 text-lg sm:text-xl text-[var(--text-secondary)] font-medium text-center leading-snug max-w-[54ch]">
            A fast and lightweight paste service for sharing text and code.
          </p>

          <p className="m-0 text-base sm:text-lg text-[var(--text-secondary)] text-center leading-relaxed max-w-[60ch]">
            SharePaste is a simple pastebin-style service built for speed,
            readability, and ease of sharing. Just paste and share your text!
          </p>

          <section
            aria-labelledby="reporting-terms-heading"
            className="w-full bg-[var(--bg-elevated)] border border-[var(--border-control)] rounded-xl p-6 sm:p-8 mt-10 mb-8 shadow-[0_8px_24px_rgba(0,0,0,0.32)] text-[var(--text-secondary)] leading-relaxed"
          >
            <h2
              id="reporting-terms-heading"
              className="mt-0 mb-3 pb-3 text-xl sm:text-2xl font-bold text-[var(--text-primary)] border-b border-[var(--border-subtle)]"
            >
              Reporting &amp; Terms
            </h2>

            <p className="mt-0 mb-4 text-sm sm:text-base">
              Content hosted on SharePaste is user-generated. Illegal content,
              abuse, or copyright violations are not permitted or encouraged.
            </p>

            <ol role="list" className="list-none p-0 m-0 space-y-3 text-sm sm:text-base">
              <li>
                <strong className="font-semibold text-[var(--text-primary)]">
                  1. No Illegal Use:{" "}
                </strong>
                You may not use SharePaste to share, store, or distribute any
                content that is illegal, harmful, or violates any laws or
                regulations.
              </li>
              <li>
                <strong className="font-semibold text-[var(--text-primary)]">
                  2. No Malicious Content:{" "}
                </strong>
                Do not upload or share content intended to harm others,
                including malware, phishing links, or personal data without
                consent.
              </li>
              <li>
                <strong className="font-semibold text-[var(--text-primary)]">
                  3. Content Responsibility:{" "}
                </strong>
                You are solely responsible for the content you post. SharePaste
                does not pre-screen content and is not liable for what users
                choose to share.
              </li>
              <li>
                <strong className="font-semibold text-[var(--text-primary)]">
                  4. Moderation:{" "}
                </strong>
                We reserve the right to remove any content at our discretion and
                to restrict or terminate access for abuse or violations of these
                terms.
              </li>
              <li>
                <strong className="font-semibold text-[var(--text-primary)]">
                  5. No Warranties or Liability:{" "}
                </strong>
                SharePaste is provided “as is” with no guarantees. We do not
                guarantee uptime, data retention, privacy, or availability.
              </li>
            </ol>

            <p className="pt-4 mt-5 mb-3 border-t border-[var(--border-subtle)] text-sm sm:text-base">
              By using SharePaste, you accept these terms. If you do not agree,
              please do not use the service.
            </p>

            <p className="m-0 text-sm sm:text-base text-[var(--accent-danger)] font-medium">
              If you encounter content that violates the Terms of Service or
              applicable laws, please report it! Please contact the site
              administrator.
            </p>
          </section>

          <p className="m-0 text-center text-sm sm:text-base text-[var(--text-secondary)] leading-relaxed max-w-[60ch]">
            This SharePaste instance is self-hosted. Please contact the server
            administrator for support or to report content:{" "}
            <strong id="ContactEmail" className="font-semibold text-[var(--text-primary)]">
              {contactEmail ? (
                <a href={`mailto:${contactEmail}`} className="sp-link">
                  {contactEmail}
                </a>
              ) : (
                "None provided."
              )}
            </strong>
          </p>
        </div>
      </main>

      <Footer />
    </div>
  );
}

export default About;
