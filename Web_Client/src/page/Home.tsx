import { useRef, useLayoutEffect, useCallback, useState, useEffect } from "react";
import Footer from "../ui/footer";
import Header from "../ui/header";
import { clsx } from "clsx";

function getUsableWidth(el: HTMLElement) {
  const style = window.getComputedStyle(el);
  const padding = parseFloat(style.paddingLeft) + parseFloat(style.paddingRight);
  const scrollbar = el.offsetWidth - el.clientWidth;
  return el.clientWidth - padding - scrollbar;
}

function getCharWidth(font: string) {
  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");
  if (!ctx) return 8; // Safest monospace fallback
  ctx.font = font;
  return ctx.measureText("M").width;
}

function Home() {

  // line numbers and text wrapping
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const lineNumbersRef = useRef<HTMLDivElement>(null);
  const [wrapEnabled, setWrapEnabled] = useState(true);

  const updateLineNumbers = useCallback(() => {
    const textarea = textareaRef.current;
    const lineNumbers = lineNumbersRef.current;
    if (!textarea || !lineNumbers) return;

    const lines = textarea.value.split("\n");
    const displayLines: (number | string)[] = [];

    const style = window.getComputedStyle(textarea);
    const font = `${style.fontSize} ${style.fontFamily}`;
    const charWidth = getCharWidth(font);
    const usableWidth = getUsableWidth(textarea);

    const charsPerLine = Math.max(1, Math.floor(usableWidth / charWidth));
    let lineNumber = 1;

    lines.forEach(line => {
      // If wrap is disabled, each line counts as 1 visual line
      const wraps = wrapEnabled ? Math.max(1, Math.ceil(line.length / charsPerLine)) : 1;

      displayLines.push(lineNumber++);
      for (let i = 1; i < wraps; i++) displayLines.push("");
    });

    lineNumbers.textContent = displayLines.join("\n");
  }, [wrapEnabled]);

  const handleScroll = useCallback(() => {
    if (textareaRef.current && lineNumbersRef.current) {
      lineNumbersRef.current.scrollTop = textareaRef.current.scrollTop;
    }
  }, []);

  useLayoutEffect(() => {
    updateLineNumbers();
    window.addEventListener("resize", updateLineNumbers);
    return () => window.removeEventListener("resize", updateLineNumbers);
  }, [updateLineNumbers]);

  const toggleWrap = () => {
    console.log(wrapEnabled ? "Disabling wrap" : "Enabling wrap");
    setWrapEnabled(prev => !prev);
  };

  // Share Button
  const handleShare = () => {

  };

  // Share Link Button
  // const handlesharelink = () => {

  // };

  // New Button
  // const handleNew = () => {
  //   if (textareaRef.current) {
  //     textareaRef.current.value = "";
  //     updateLineNumbers();
  //   }
  // };

  
  // Check for Paste on load

  // Get Max Paste Size from server on load
  // const getMaxPasteSize = async () => {
  //   const textLimit = sessionStorage.getItem("textLimit");
  //   const pasteBox = textareaRef.current!;

  //   if (textLimit) {
  //     const limit = parseInt(textLimit, 10);
  //     pasteBox.maxLength = limit;
  //     console.log("Max paste size set from sessionStorage:", textLimit);
  //     return;
  //   }

  //   try {
  //     const response = await fetch("/api/maxsize");
  //     const maxPasteSize = await response.text();
  //     sessionStorage.setItem("textLimit", maxPasteSize);
  //     pasteBox!.maxLength = parseInt(maxPasteSize, 10);
    
  //     console.log("Max paste size set from server:", maxPasteSize);
    
  //   } catch (error) {
  //     console.error("Failed to fetch max paste size:", error);
  //   }
  // };

  // useEffect(() => {
  //   getMaxPasteSize();
  // }, []);

  // Tab Indent support

  return (
    <div className="flex flex-col h-screen">
      <Header wrapFunc={toggleWrap}/>

      <div className="flex flex-1 w-full overflow-hidden">
        <div 
          ref={lineNumbersRef}
          className="bg-[#292929] text-[#b9b9b9] text-right select-none overflow-hidden font-bold whitespace-pre font-mono text-[16px] p-2.5 leading-[1.4em] min-w-[2.5rem]"
        >
          1
        </div>
        <textarea
          ref={textareaRef}
          wrap={wrapEnabled ? "soft" : "off"}
          className={clsx("bg-[#1f1f22] text-[#d6d6d6] w-full flex-1 resize-none focus:outline-none font-mono text-[16px] p-2.5 leading-[1.4em] overflow-y-auto",
            {
              "overflow-x-auto whitespace-pre": !wrapEnabled,
              "overflow-x-hidden whitespace-pre-wrap": wrapEnabled,
            }
          )}
          spellCheck={false}
          maxLength={5000000}
          onChange={updateLineNumbers}
          onScroll={handleScroll}
        ></textarea>
      </div>

      <Footer />
    </div>
  );
}

export default Home;