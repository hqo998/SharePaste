import { useState } from "react";

const FOOTER_STORAGE_KEY = "footerVisible";

export default function Footer() {
  const [isVisible, setIsVisible] = useState(() => {
    try {
      return (
        sessionStorage.getItem(FOOTER_STORAGE_KEY) !== "false" &&
        sessionStorage.getItem("isFooterClosed") !== "true"
      );
    } catch {
      return true;
    }
  });

  const hideFooter = () => {
    setIsVisible(false);
    try {
      sessionStorage.setItem(FOOTER_STORAGE_KEY, "false");
    } catch {
      // Ignore storage errors in private browsing
    }
  };

  if (!isVisible) {
    return null;
  }

  return (
    <footer
      id="footerBar"
      className="flex items-center justify-between gap-3 px-3 py-2 bg-[var(--bg-chrome)] border-t border-[var(--border-subtle)] overflow-x-auto whitespace-nowrap"
    >
      <p className="m-0 mx-auto text-sm sm:text-[15px] font-medium text-[var(--text-secondary)]">
        <span>Check out </span>
        <a
          href="https://github.com/hqo998/SharePaste"
          target="_blank"
          rel="noreferrer"
          className="sp-link font-semibold"
        >
          SharePaste on GitHub
        </a>
        <span>!</span>
      </p>

      <button
        id="footerCloseButton"
        type="button"
        className="sp-btn text-sm py-1 px-3.5"
        onClick={hideFooter}
        aria-label="Dismiss footer banner"
      >
        Close
      </button>
    </footer>
  );
}
