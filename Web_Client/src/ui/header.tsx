import { useState } from "react";
import { NavLink, useLocation } from "react-router-dom";

interface HeaderProps {
  wrapEnabled?: boolean;
  wrapFunc?: () => void;
  newHandle?: () => void;
  shareHandle?: () => void;
  isSharing?: boolean;
  shareState?: "idle" | "ready" | "error";
  viewCountRef?: React.RefObject<HTMLSpanElement | null>;
  shareLinkRef?: React.RefObject<HTMLInputElement | null>;
}

export default function Header({
  wrapEnabled = true,
  wrapFunc,
  newHandle,
  shareHandle,
  isSharing = false,
  shareState = "idle",
  viewCountRef,
  shareLinkRef,
}: HeaderProps) {
  const location = useLocation();
  const isAboutPage = location.pathname === "/about";
  const [isCopied, setIsCopied] = useState(false);

  const handleShareLink = async (e: React.MouseEvent<HTMLInputElement>) => {
    const input = e.currentTarget;
    if (!input.value || isCopied) return;

    // Only copy valid share URLs, not error messages
    if (!input.value.startsWith("http")) return;

    try {
      const originalUrl = input.value;
      await navigator.clipboard.writeText(originalUrl);
      setIsCopied(true);
      input.value = "Copied!";

      window.setTimeout(() => {
        if (input.value === "Copied!") {
          input.value = originalUrl;
        }
        setIsCopied(false);
      }, 900);
    } catch (error) {
      console.error("Failed to copy share link:", error);
    }
  };

  if (isAboutPage) {
    return (
      <header className="flex items-center justify-between gap-2.5 px-3 py-2 bg-[var(--bg-chrome)] border-b border-[var(--border-subtle)]">
        <NavLink
          to="/"
          className="flex items-center gap-2 h-8 px-1 rounded no-underline shrink-0"
          title="Return to SharePaste editor"
          aria-label="SharePaste Home"
        >
          <img
            src="/www/favicon.svg"
            alt=""
            width={28}
            height={28}
            className="h-7 w-7 block object-contain"
          />
          <span className="text-sm font-bold tracking-tight text-[var(--text-primary)] hidden sm:inline">
            SharePaste
          </span>
        </NavLink>

        <nav aria-label="Primary" className="flex items-center gap-2.5">
          <NavLink to="/" className="sp-btn">
            Editor
          </NavLink>
        </nav>
      </header>
    );
  }

  const inputVisualState = isCopied
    ? "copied"
    : shareState === "error"
      ? "error"
      : shareState;

  return (
    <header className="sp-scroller flex items-center gap-2.5 px-3 py-2 bg-[var(--bg-chrome)] border-b border-[var(--border-subtle)] flex-nowrap overflow-x-auto">
      <NavLink
        to="/"
        onClick={newHandle}
        className="flex items-center justify-center h-8 w-8 rounded no-underline shrink-0"
        title="SharePaste Home"
        aria-label="SharePaste Home"
      >
        <img
          src="/www/favicon.svg"
          alt=""
          width={28}
          height={28}
          className="h-7 w-7 block object-contain"
        />
      </NavLink>

      <button
        id="newButton"
        type="button"
        className="sp-btn"
        onClick={newHandle}
        title="Start a new paste"
      >
        New
      </button>

      <button
        id="shareButton"
        type="button"
        className="sp-btn"
        onClick={shareHandle}
        disabled={isSharing}
        title="Save paste and generate a shareable link"
      >
        {isSharing ? "Sharing…" : "Share"}
      </button>

      <input
        id="shareLink"
        className="sp-share-input w-56 sm:w-72 shrink-0 read-only:cursor-pointer"
        type="text"
        readOnly
        placeholder="Share URL appears here…"
        aria-label="Shareable paste URL (click to copy)"
        title="Click to copy shareable link"
        data-state={inputVisualState}
        ref={shareLinkRef}
        onClick={handleShareLink}
      />

      <div className="ml-auto flex gap-2.5 items-center shrink-0 pl-1">
        <button
          id="wrapButton"
          type="button"
          className="sp-btn"
          aria-pressed={wrapEnabled}
          onClick={wrapFunc}
          title={wrapEnabled ? "Line wrapping enabled (click to disable)" : "Line wrapping disabled (click to enable)"}
        >
          Wrap
        </button>

        <NavLink to="/about" className="sp-btn">
          About
        </NavLink>

        <div
          className="inline-flex items-center gap-1.5 px-2 py-1 rounded text-[15px] font-semibold text-[var(--text-secondary)] whitespace-nowrap select-none tabular-nums"
          title="Paste view count"
          aria-label="Paste view count"
        >
          <svg
            aria-hidden="true"
            viewBox="0 0 24 24"
            fill="none"
            stroke="currentColor"
            strokeWidth="2"
            strokeLinecap="round"
            strokeLinejoin="round"
            className="w-4 h-4 text-[var(--accent-amber)] shrink-0"
          >
            <path d="M2.062 12.348a1 1 0 0 1 0-.696 10.75 10.75 0 0 1 19.876 0 1 1 0 0 1 0 .696 10.75 10.75 0 0 1-19.876 0" />
            <circle cx="12" cy="12" r="3" />
          </svg>
          <span id="viewCount" ref={viewCountRef}>
            0
          </span>
        </div>
      </div>
    </header>
  );
}
