import {
  useRef,
  useLayoutEffect,
  useCallback,
  useState,
  useEffect,
  type KeyboardEvent,
} from "react";
import { clsx } from "clsx";
import Footer from "../ui/footer";
import Header from "../ui/header";

const TAB_SPACES = "  ";
const MAX_SIZE_STORAGE_KEY = "TextLimit";

let cachedMeasureCtx: CanvasRenderingContext2D | null = null;

function getUsableWidth(el: HTMLElement) {
  const style = window.getComputedStyle(el);
  const padding =
    parseFloat(style.paddingLeft) + parseFloat(style.paddingRight);
  const scrollbar = el.offsetWidth - el.clientWidth;
  return el.clientWidth - padding - scrollbar;
}

function getCharWidth(font: string) {
  if (!cachedMeasureCtx) {
    const canvas = document.createElement("canvas");
    cachedMeasureCtx = canvas.getContext("2d");
  }
  if (!cachedMeasureCtx) return 8;
  cachedMeasureCtx.font = font;
  return cachedMeasureCtx.measureText("M").width || 8;
}

function Home() {
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const lineNumbersRef = useRef<HTMLDivElement>(null);
  const shareLinkRef = useRef<HTMLInputElement>(null);
  const viewCountRef = useRef<HTMLSpanElement>(null);

  const [textContent, setTextContent] = useState("");
  const [maxPasteSize, setMaxPasteSize] = useState(5000000);
  const [wrapEnabled, setWrapEnabled] = useState(true);
  const [isSharing, setIsSharing] = useState(false);
  const [shareState, setShareState] = useState<"idle" | "ready" | "error">(
    "idle"
  );

  // Escape-then-Tab allows keyboard-only users to move focus out of the editor
  const tabTrappedRef = useRef(true);

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

    lines.forEach((line) => {
      const wraps = wrapEnabled
        ? Math.max(1, Math.ceil(line.length / charsPerLine))
        : 1;

      displayLines.push(lineNumber++);
      for (let i = 1; i < wraps; i++) {
        displayLines.push("");
      }
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
    setWrapEnabled((prev) => !prev);
  };

  const handleNew = () => {
    if (textareaRef.current) {
      textareaRef.current.value = "";
      textareaRef.current.focus();
      updateLineNumbers();
    }

    if (shareLinkRef.current) {
      shareLinkRef.current.value = "";
    }

    if (viewCountRef.current) {
      viewCountRef.current.textContent = "0";
    }

    setTextContent("");
    setShareState("idle");

    const currentHash = window.location.hash;
    window.history.pushState({}, "", "/" + (currentHash === "#/" ? "" : ""));
  };

  useEffect(() => {
    document.title = "SharePaste";
  }, []);

  useEffect(() => {
    const checkForPaste = async () => {
      const uniqueCode = window.location.pathname.slice(1).trim();
      if (!uniqueCode || uniqueCode === "about") return;

      const shareUrl = `${window.location.origin}/${uniqueCode}`;

      try {
        const storageData = sessionStorage.getItem(uniqueCode);
        if (storageData) {
          const pasteDataObj = JSON.parse(storageData);
          if (textareaRef.current) {
            textareaRef.current.value = pasteDataObj.pasteBody ?? "";
          }
          if (viewCountRef.current) {
            viewCountRef.current.textContent = String(
              pasteDataObj.viewCount ?? 0
            );
          }
          if (shareLinkRef.current) {
            shareLinkRef.current.value = shareUrl;
          }
          setTextContent(pasteDataObj.pasteBody ?? "");
          setShareState("ready");
          updateLineNumbers();
          return;
        }
      } catch {
        // Proceed to network fetch if sessionStorage read/parse fails
      }

      try {
        const response = await fetch(
          `/api/find?code=${encodeURIComponent(uniqueCode)}`
        );

        if (response.status === 418 || response.status === 404) {
          if (textareaRef.current) {
            textareaRef.current.value = "404 - Paste not found";
          }
          if (viewCountRef.current) {
            viewCountRef.current.textContent = "0";
          }
          updateLineNumbers();
          return;
        }

        if (response.status === 429) {
          if (shareLinkRef.current) {
            shareLinkRef.current.value = "Too many requests...";
          }
          setShareState("error");
          return;
        }

        if (!response.ok) {
          return;
        }

        const contentType = response.headers.get("content-type") || "";
        const rawText = await response.text();
        if (
          rawText.trimStart().toLowerCase().startsWith("<!doctype html") ||
          (!contentType.includes("json") && !rawText.trimStart().startsWith("{"))
        ) {
          return;
        }

        const pasteData = JSON.parse(rawText);
        const body = pasteData.pasteBody ?? "";
        const views = pasteData.viewCount ?? 0;

        if (textareaRef.current) {
          textareaRef.current.value = body;
        }
        if (viewCountRef.current) {
          viewCountRef.current.textContent = String(views);
        }
        if (shareLinkRef.current) {
          shareLinkRef.current.value = shareUrl;
        }

        setTextContent(body);
        setShareState("ready");

        try {
          sessionStorage.setItem(
            uniqueCode,
            JSON.stringify({ pasteBody: body, viewCount: views })
          );
        } catch {
          // Ignore storage quota errors
        }
      } catch (error) {
        console.error("Error fetching paste data:", error);
      } finally {
        updateLineNumbers();
      }
    };

    checkForPaste();
  }, [updateLineNumbers]);

  const handleShare = async () => {
    if (!textareaRef.current || isSharing) return;
    const pasteContent = textareaRef.current.value;

    if (!pasteContent.trim()) return;

    if (
      pasteContent === textContent &&
      shareState === "ready" &&
      shareLinkRef.current?.value.startsWith("http")
    ) {
      return;
    }

    if (pasteContent.length > maxPasteSize) {
      if (shareLinkRef.current) {
        shareLinkRef.current.value = `Limit: ${maxPasteSize.toLocaleString()} chars`;
      }
      setShareState("error");
      return;
    }

    setIsSharing(true);

    try {
      const response = await fetch("/api/new", {
        method: "POST",
        headers: {
          "Content-Type": "application/json; charset=UTF-8",
        },
        body: JSON.stringify({ pasteBody: pasteContent }),
      });

      const responseText = (await response.text()).trim();

      if (response.status === 429) {
        if (shareLinkRef.current) {
          shareLinkRef.current.value = "Too many requests...";
        }
        setShareState("error");
        return;
      }

      if (
        !response.ok ||
        !responseText ||
        responseText.toLowerCase().startsWith("<!doctype html") ||
        responseText.startsWith("Request Invalid")
      ) {
        if (shareLinkRef.current) {
          shareLinkRef.current.value =
            responseText && responseText.length < 40
              ? responseText
              : "Error - please try again";
        }
        setShareState("error");
        return;
      }

      const fullShareUrl = `${window.location.origin}/${responseText}`;

      if (shareLinkRef.current) {
        shareLinkRef.current.value = fullShareUrl;
      }
      if (viewCountRef.current) {
        viewCountRef.current.textContent = "0";
      }

      setTextContent(pasteContent);
      setShareState("ready");

      try {
        sessionStorage.setItem(
          responseText,
          JSON.stringify({ pasteBody: pasteContent, viewCount: 0 })
        );
      } catch {
        // Ignore storage quota errors
      }

      window.history.pushState({}, "", `/${responseText}`);
    } catch (error) {
      console.error("Failed to create new paste:", error);
      if (shareLinkRef.current) {
        shareLinkRef.current.value = "Error - please try again";
      }
      setShareState("error");
    } finally {
      setIsSharing(false);
    }
  };

  useEffect(() => {
    const getMaxPasteSize = async () => {
      try {
        const cachedLimit = sessionStorage.getItem(MAX_SIZE_STORAGE_KEY);
        if (cachedLimit) {
          const parsedCached = parseInt(cachedLimit, 10);
          if (!isNaN(parsedCached) && parsedCached > 0) {
            setMaxPasteSize(parsedCached);
            return;
          }
        }
      } catch {
        // Ignore storage read errors
      }

      try {
        const response = await fetch("/api/maxsize");
        if (!response.ok) return;
        const maxLimit = await response.text();

        const parsedLimit = parseInt(maxLimit, 10);
        if (isNaN(parsedLimit) || parsedLimit <= 0) {
          return;
        }

        setMaxPasteSize(parsedLimit);
        try {
          sessionStorage.setItem(MAX_SIZE_STORAGE_KEY, String(parsedLimit));
        } catch {
          // Ignore storage write errors
        }
      } catch {
        // Keep default client paste size limit when running standalone
      }
    };

    getMaxPasteSize();
  }, []);

  const handleKeyDown = (e: KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === "Escape") {
      tabTrappedRef.current = false;
      return;
    }

    if (e.key !== "Tab") {
      tabTrappedRef.current = true;
      return;
    }

    if (e.ctrlKey || e.metaKey || e.altKey || !tabTrappedRef.current) {
      tabTrappedRef.current = true;
      return;
    }

    e.preventDefault();
    const textarea = e.currentTarget;
    const start = textarea.selectionStart;
    const end = textarea.selectionEnd;
    const value = textarea.value;

    // Single cursor without Shift: insert 2-space indent at caret
    if (start === end && !e.shiftKey) {
      textarea.value =
        value.slice(0, start) + TAB_SPACES + value.slice(end);
      textarea.selectionStart = textarea.selectionEnd =
        start + TAB_SPACES.length;
      updateLineNumbers();
      return;
    }

    // Expand selection to full lines
    const lineStart = value.lastIndexOf("\n", start - 1) + 1;
    let lineEnd = value.indexOf("\n", end);
    if (lineEnd === -1) lineEnd = value.length;

    const selectedText = value.slice(lineStart, lineEnd);
    const lines = selectedText.split("\n");

    if (e.shiftKey) {
      for (let i = 0; i < lines.length; i++) {
        if (lines[i].startsWith(TAB_SPACES)) {
          lines[i] = lines[i].slice(TAB_SPACES.length);
        } else if (lines[i].startsWith("\t")) {
          lines[i] = lines[i].slice(1);
        } else if (lines[i].startsWith(" ")) {
          lines[i] = lines[i].replace(/^ {1,2}/, "");
        }
      }
    } else {
      for (let i = 0; i < lines.length; i++) {
        lines[i] = TAB_SPACES + lines[i];
      }
    }

    const updatedBlock = lines.join("\n");
    textarea.value =
      value.slice(0, lineStart) + updatedBlock + value.slice(lineEnd);
    textarea.selectionStart = lineStart;
    textarea.selectionEnd = lineStart + updatedBlock.length;
    updateLineNumbers();
  };

  return (
    <div className="flex flex-col h-dvh bg-[var(--bg-canvas)] text-[var(--text-primary)]">
      <Header
        wrapEnabled={wrapEnabled}
        wrapFunc={toggleWrap}
        newHandle={handleNew}
        shareHandle={handleShare}
        isSharing={isSharing}
        shareState={shareState}
        viewCountRef={viewCountRef}
        shareLinkRef={shareLinkRef}
      />

      <main className="flex flex-1 w-full overflow-hidden">
        <div
          id="lineNumbers"
          ref={lineNumbersRef}
          aria-hidden="true"
          className="sp-gutter text-right select-none overflow-hidden font-bold whitespace-pre text-[16px] py-2.5 px-3 leading-[1.45] min-w-[3.25rem]"
        >
          1
        </div>
        <textarea
          id="pasteBox"
          ref={textareaRef}
          wrap={wrapEnabled ? "soft" : "off"}
          aria-label="Paste text editor"
          placeholder="Paste or type code here…"
          className={clsx(
            "sp-editor-textarea sp-scroller w-full flex-1 resize-none text-[16px] p-2.5 leading-[1.45] overflow-y-auto border-0",
            {
              "overflow-x-auto whitespace-pre": !wrapEnabled,
              "overflow-x-hidden whitespace-pre-wrap break-words": wrapEnabled,
            }
          )}
          spellCheck={false}
          autoCapitalize="off"
          autoCorrect="off"
          maxLength={maxPasteSize}
          onChange={updateLineNumbers}
          onKeyDown={handleKeyDown}
          onScroll={handleScroll}
        />
      </main>

      <Footer />
    </div>
  );
}

export default Home;
