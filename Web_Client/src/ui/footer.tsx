import { useState, useEffect } from "react";

export default function Footer() {
  const [ isVisible, setIsVisible] = useState(true);

  useEffect(() => {
    const storageData = sessionStorage.getItem("footerVisible");
    if (storageData === "false") {
      setIsVisible(false);
    }
  }, []);

  const hideFooter = () => {
    setIsVisible(false);
    sessionStorage.setItem("footerVisible", "false");
  };

  if (!isVisible) {
    return null;
  }
  return (
    <footer id="footerBar" className="flex items-center gap-3 p-3 bg-[#0d0d0d] overflow-x-auto whitespace-nowrap">
      <span className="mx-auto text-[15px] font-bold font-sans opacity-80 text-white">
        <span>Check out </span>
        <a href="https://github.com/hqo998/SharePaste" target="_blank" rel="noreferrer" className="text-blue-500 hover:underline">
          SharePaste on GitHub
        </a>
        <span>!</span>
      </span>
      
      <button
        id="footerCloseButton"
        className="bg-[#323232] hover:bg-[#3c3c3c] border border-[#444444] text-[#d6d6d6] font-bold px-6 py-1.5 rounded-md transition-all active:top-px relative shadow-[inset_0_1px_0_0_#323232]"
        onClick={hideFooter}
      >
        Close
      </button>
    </footer>
  );
}