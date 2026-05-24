import { useRef, useLayoutEffect, useCallback, useState, useEffect } from "react";
import Footer from "../ui/footer";
import Header from "../ui/header";
import { clsx } from "clsx";
// import { useNavigate } from "react-router-dom";

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
  // const navigate = useNavigate();
  
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const [textContent, setTextContent] = useState("");
  const [maxPasteSize, setMaxPasteSize] = useState(5000000);

  // header references
  const shareLinkRef = useRef<HTMLInputElement>(null!);
  const viewCountRef = useRef<HTMLSpanElement>(null!);

  // line numbers and text wrapping
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


  // TODO: New Button and pass to func to button
  const handleNew = () => {
    if (textareaRef && textareaRef.current) {
      textareaRef.current.value = "";
      updateLineNumbers();
    }

    if (shareLinkRef && shareLinkRef.current) {
      shareLinkRef.current.value = "";
    }

    if (viewCountRef && viewCountRef.current) {
       viewCountRef.current.textContent = "👁";
    }

    setTextContent(""); // wipe prev tracker to clear.
    
    // redirect("/"); // Clear URL and state for new paste
    window.history.pushState({}, "", "/");
  };
  
  // TODO: Check for Paste on load
  useEffect(() => {
    const checkForPaste = async () => {
      // console.log("Checking for existing paste in URL...");

      const uniqueCode = window.location.pathname.slice(1);
      const storageData = sessionStorage.getItem(uniqueCode);

      if (!uniqueCode) return;

      if (storageData) {
        // console.log("Found session storage data for code:", uniqueCode);
        const pasteDataObj = JSON.parse(storageData);
        textareaRef.current!.value = pasteDataObj.pasteBody;
        viewCountRef.current!.textContent = pasteDataObj.viewCount ? `👁 ${pasteDataObj.viewCount.toString()}` : "👁 0";
        shareLinkRef.current!.value = document.URL;
        setTextContent(pasteDataObj.pasteBody);

        updateLineNumbers();
        return;
      }

      try {
        const response = await fetch(`/api/find?code=${encodeURIComponent(uniqueCode)}`);
        if (!response.ok) {
          console.error("Failed to fetch paste data:", response.statusText);
          return;
        }
        const pasteData = await response.json();

        // console.log("Fetched paste data from server for code:", uniqueCode, pasteData);
        textareaRef.current!.value = pasteData.pasteBody;
        viewCountRef.current!.textContent = pasteData.viewCount ? `👁 ${pasteData.viewCount.toString()}` : "👁 0";
        shareLinkRef.current!.value = document.URL;
        setTextContent(pasteData.pasteBody);
         
        const localDataObj = {
          pasteBody: pasteData.pasteBody,
          viewCount: pasteData.viewCount || 0,
        };
        sessionStorage.setItem(uniqueCode, JSON.stringify(localDataObj) );

      } catch (error) {
        console.error("Error fetching paste data:", error);
      
      } finally {
        updateLineNumbers();
      }
    };

    checkForPaste();
  }, [updateLineNumbers]);

  // TODO: Sharebutton
  const handleShare = async () => {
    if (!textareaRef.current) return;
    const pasteContent = textareaRef.current.value;

    if (!pasteContent.trim()) return; // no empty pastes

    if (pasteContent == textContent) return; // no reshares without changes

    if (pasteContent.length > maxPasteSize) {
      alert(`Paste exceeds maximum size of ${maxPasteSize} characters.`);
      return;
    }

    const origin = window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' + window.location.port : '');

    const response = await fetch("/api/new",
      {
        method: "POST",
        headers:
      {
        "Content-type": "application/json; charset=UTF-8"
      },
        body: JSON.stringify({ pasteBody: pasteContent })
      })

    if (!response.ok) {
      console.error("Failed to create new paste:", response.statusText);
      if (shareLinkRef.current) {
        shareLinkRef.current.value = "Error - please try again";
      }
      return;
    }

    const data = await response.text();

    if (shareLinkRef.current) {
      shareLinkRef.current.value = `${origin}/${data}`;
      setTextContent(pasteContent);
    }

    // navigate("/" + data);
    window.history.pushState({}, "", data);

  }


  // TODO: Get Max Paste Size from server on load
  useEffect(() => {
    const getMaxPasteSize = async () => {

    try {
      const response = await fetch("/api/maxsize");
      const maxLimit = await response.text();

      const parsedLimit = parseInt(maxLimit, 10);
      if (isNaN(parsedLimit) || parsedLimit <= 0) {
        console.warn("Invalid max paste size received from server, using default:", maxLimit);
        return;
      }

      setMaxPasteSize(parsedLimit);
    
    } catch (error) {
      console.error("Failed to fetch max paste size:", error);
    }
    };

    getMaxPasteSize();
  }, []);

  // TODO: Tab Indent support



  
  return (
    <div className="flex flex-col h-screen">
      <Header wrapFunc={toggleWrap} newHandle={handleNew} shareHandle={handleShare} viewCountRef={viewCountRef} shareLinkRef={shareLinkRef} />

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
          maxLength={maxPasteSize}
          onChange={updateLineNumbers}
          onScroll={handleScroll}
        ></textarea>
      </div>

      <Footer />
    </div>
  );
}

export default Home;