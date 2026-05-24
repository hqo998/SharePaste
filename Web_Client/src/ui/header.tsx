import { NavLink, useLocation } from "react-router-dom";

interface HeaderProps {
  wrapFunc?: () => void;
  newHandle?: () => void;
  shareHandle?: () => void;
  viewCountRef?: React.RefObject<HTMLSpanElement>;
  shareLinkRef?: React.RefObject<HTMLInputElement>;
}


export default function Header({wrapFunc, newHandle, shareHandle, viewCountRef, shareLinkRef}: HeaderProps) {
  const location = useLocation();
  const isAboutPage = location.pathname === "/about";

  const handleShareLink = async () => {
    if (shareLinkRef && shareLinkRef.current && shareLinkRef.current.value) {
      try {
        shareLinkRef.current.disabled = true; // Disable the input to prevent multiple clicks
        await navigator.clipboard.writeText(shareLinkRef.current.value);
        const old = shareLinkRef.current.value;
        shareLinkRef.current.value = "Copied!";
        setTimeout(() => {
          if (shareLinkRef.current) {
            shareLinkRef.current.value = old;
          }
        }, 500);

        shareLinkRef.current.disabled = false; // Re-enable the input

      } catch (error) {
        console.error("Failed to copy share link:", error);
      }
    }
  };

  if (!isAboutPage) {
    // Home page header and everything else.
    return (
    <header className="flex items-center gap-2.5 p-2.5 bg-[#0d0d0d] flex-nowrap overflow-x-auto max-sm:flex-nowrap">
      <NavLink to="/" className="flex items-center justify-center h-6 no-underline" title="Home">
        <img src="/www/favicon.svg" alt="Home" className="h-[130%] w-auto block cursor-pointer" />
      </NavLink>

      <button
        id="newButton"
        className="cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5"
        onClick={newHandle}
      >
        New
      </button>

      <button id="shareButton" className="cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5" onClick={shareHandle}>
        Share
      </button>

      <input
        id="shareLink"
        className="bg-[#323232] text-[#d6d6d6] border border-[#444444] px-2.5 py-1.5 rounded w-75 font-mono focus:outline-none read-only:cursor-default hover:cursor-text active:border-white transition-all duration-100"
        type="text"
        readOnly
        ref={shareLinkRef}
        onClick={handleShareLink}
      />

      <div className="ml-auto flex gap-2.5 items-center">
        <NavLink
          to="about"
          className="inline-block cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5"
          >
            About
        </NavLink>

        <button
          id="wrapButton"
          className="cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5" 
          onClick={wrapFunc}>
            Wrap
        </button>

        <span
          id="viewCount"
          className="opacity-80 whitespace-nowrap font-sans text-[15px] font-bold text-white"
          ref={viewCountRef}
          >
          👁 0
        </span>
      </div>
    </header>
    );
  } else {
    // About page header
    return (  
      <header className="flex items-center gap-2.5 p-2.5 bg-[#0d0d0d] flex-nowrap overflow-x-auto max-sm:flex-nowrap">
        <NavLink to="/" className="flex items-center justify-center h-6 no-underline" title="Home">
            <img src="/www/favicon.svg" alt="Home" className="h-[130%] w-auto block cursor-pointer" />
        </NavLink>
        <div className="ml-auto flex gap-2.5 items-center">
            <NavLink
              to="/"
              className="inline-block cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5"
            >
              Home
            </NavLink>
        </div>
      </header>
    );
  }
}