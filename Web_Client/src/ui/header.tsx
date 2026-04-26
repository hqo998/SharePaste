import { NavLink, useLocation } from "react-router-dom";

export default function Header({wrapFunc}: {wrapFunc?: () => void}) {
  const location = useLocation();

  const isAboutPage = location.pathname === "/about";

  if (!isAboutPage) {
    // Home page header and everything else.
    return (
    <header className="flex items-center gap-2.5 p-2.5 bg-[#0d0d0d] flex-nowrap overflow-x-auto max-sm:flex-nowrap">
      <NavLink to="/" className="flex items-center justify-center h-6 no-underline" title="Home">
        <img src="/www/favicon.svg" alt="Home" className="h-[130%] w-auto block cursor-pointer" />
      </NavLink>
      <button id="newButton" className="cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5">New</button>
      <button id="shareButton" className="cursor-pointer bg-[#323232] rounded-md border border-[#444444] text-[#d6d6d6] font-sans text-[15px] font-bold px-6 py-1.5 no-underline transition-all duration-200 ease-in-out hover:bg-[#3c3c3c] active:relative active:top-px shadow-[inset_0_1px_0_0_#323232] max-sm:min-h-7.5">Share</button>

      <input id="shareLink" className="bg-[#323232] text-[#d6d6d6] border border-[#444444] px-2.5 py-1.5 rounded w-75 font-mono focus:outline-none read-only:cursor-default" type="text" readOnly />

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
        <span id="viewCount" className="opacity-80 whitespace-nowrap font-sans text-[15px] font-bold text-white">👁 0</span>
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