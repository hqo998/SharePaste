
// line numbers
const textarea = document.getElementById("pasteBox");
const lineNumbers = document.getElementById("lineNumbers");

function getUsableWidth(el) {
  const style = getComputedStyle(el);

  const padding =
    parseFloat(style.paddingLeft) +
    parseFloat(style.paddingRight);

  const scrollbar = el.offsetWidth - el.clientWidth;

  return el.clientWidth - padding - scrollbar;
}

function getCharWidth(font) {
  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");
  ctx.font = font;
  return ctx.measureText("M").width; // safest monospace glyph
}

const style = getComputedStyle(textarea);
const font = `${style.fontSize} ${style.fontFamily}`;
const charWidth = getCharWidth(font);

const usableWidth = getUsableWidth(textarea);
const charsPerLine = Math.floor(usableWidth / charWidth);

function updateLineNumbers() {
  const lines = textarea.value.split("\n");
  const displayLines = [];

  const style = getComputedStyle(textarea);
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
}

// Update on user typing
textarea.addEventListener("input", updateLineNumbers);

// Update when window is resized
window.addEventListener("resize", updateLineNumbers);

// Update scroll syncing
textarea.addEventListener("scroll", () => {
  lineNumbers.scrollTop = textarea.scrollTop;
});




let G_LASTTEXT = "";

// sharebutton
document.getElementById("shareButton").addEventListener("click", function ()
{
    const pasteRequest = document.getElementById("pasteBox").value;

    if (!pasteRequest) return; // early return for empty value

    if (pasteRequest == G_LASTTEXT && !document.getElementById("shareLink")?.value.startsWith("http")) return;
    else G_LASTTEXT = pasteRequest;

    const origin = window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' + window.location.port : '');
    fetch("/api/new",
        {
        method: "POST",
        headers:
        {
            "Content-type": "application/json; charset=UTF-8"
        },
        body: JSON.stringify({ pasteBody: pasteRequest })
        })

    .then(async (response) => {
      const text = await response.text();
      const status = response.status;
      return { text, status };
      })

    .then(({text, status}) => {
      if (status == 200) {
        document.getElementById("shareLink").value = origin + '/' + text;
      }
      else {
        document.getElementById("shareLink").value = text;
      }
    })
});

// new button
document.getElementById("newButton").addEventListener("click", function ()
{
    const emptyText = "";
    document.getElementById("pasteBox").value = emptyText;
    document.getElementById("shareLink").value = emptyText;
    // document.location.href = '/';
    document.getElementById("viewCount").value = "👁 0";
});

// share button
document.getElementById("shareLink").onclick = async function()
{
    if (this.value == "") return;

    this.disabled = true;
    const timeDelay = 500;

    try {
        await navigator.clipboard.writeText(this.value);
        const old = this.value;
        this.value = "Copied!";
        this.classList.add("shareConfirmation");
        setTimeout(() => {
            this.value = old;
            this.classList.remove("shareConfirmation");
            this.disabled = false;
        }, timeDelay);
    } catch (err) {
        console.error("Clipboard copy failed", err);
        this.disabled = false;
    }
}



// auto load paste data on page load
document.addEventListener('DOMContentLoaded', () => {
  const uniqueCode = window.location.pathname.slice(1);

  const storageData = sessionStorage.getItem(uniqueCode);

  if (!uniqueCode) return; // exit if no code

  if (storageData) { // use locally found data instead.
    console.log("Local cache found for unique code.");
    const pasteData = JSON.parse(storageData);
    document.getElementById("viewCount").textContent = "👁 " + pasteData.viewCount;
    document.getElementById('pasteBox').textContent = pasteData.pasteBody;
    document.getElementById("shareLink").value = document.URL;
    updateLineNumbers();
    return;
  }

  fetch(`/api/find?code=${encodeURIComponent(uniqueCode)}`)
    .then(res => {
      if (!res.ok) throw new Error('Paste not found');
      return res.json();
    })
    .then(data => {
      document.getElementById('pasteBox').textContent = data.pasteBody;
      document.getElementById("shareLink").value = document.URL;
      document.getElementById("viewCount").textContent = "👁 " + data.viewCount;
      const localDataObj = {
        pasteBody: data.pasteBody,
        viewCount: data.viewCount
      }
      sessionStorage.setItem(uniqueCode, JSON.stringify(localDataObj));

      updateLineNumbers();
    })
    .catch(err => {
      document.getElementById('pasteBox').textContent = '';
      console.error(err);
    });
});


// footer close
document.getElementById("footerCloseButton").addEventListener("click", function ()
{
    document.getElementById("footerBar").classList.add("hidden");
    sessionStorage.setItem("isFooterClosed", "true");
});

// stay closed on reload
document.addEventListener('DOMContentLoaded', () => {
  const storageData = sessionStorage.getItem("isFooterClosed");
  if (storageData == "true") {
    document.getElementById("footerBar").classList.add("hidden");
    sessionStorage.setItem("isFooterClosed", "true");
  }
});

// tab indents
document.getElementById('pasteBox').addEventListener('keydown', function(e) { // mostly ai - rewrite
  if (e.key === 'Tab' && !(e.ctrlKey)) {
    e.preventDefault();

    let start = this.selectionStart;
    let end = this.selectionEnd;
    const value = this.value;
    const tab = '  '; // 2 spaces for tabs - sue me

    // No selection, simple tab insertion at cursor
    if (start === end && !e.shiftKey) {
      this.value = value.slice(0, start) + tab + value.slice(end);
      this.selectionStart = this.selectionEnd = start + tab.length;
      return;
    }

    // Expand selection to full lines based of \n
    let lineStart = value.lastIndexOf('\n', start - 1) + 1;
    let lineEnd = value.indexOf('\n', end);
    if (lineEnd === -1) lineEnd = value.length;

    const selectedText = value.slice(lineStart, lineEnd);
    const lines = selectedText.split('\n');

    if (e.shiftKey) {
      // SHIFT+TAB: remove indentation
      for (let i = 0; i < lines.length; i++) {
        if (lines[i].startsWith(tab)) {
          lines[i] = lines[i].slice(tab.length);
        } else if (lines[i].startsWith(' ')) {
          // remove any leading spaces if less than tab
          lines[i] = lines[i].replace(/^ +/, '');
        }
      }
    } else {
      // TAB: add indentation
      for (let i = 0; i < lines.length; i++) {
        lines[i] = tab + lines[i];
      }
    }

    const newText = lines.join('\n');

    this.value = value.slice(0, lineStart) + newText + value.slice(lineEnd);

    // Adjust selection to cover modified lines
    this.selectionStart = lineStart;
    this.selectionEnd = lineStart + newText.length;
  }
});


let wrapEnabled = true;

function toggleWrap() {
  wrapEnabled = !wrapEnabled;

  if (wrapEnabled) {
    textarea.style.whiteSpace = "pre-wrap";
    textarea.style.wordBreak = "break-word";
    textarea.wrap = "soft";
  } else {
    textarea.style.whiteSpace = "pre";
    textarea.style.wordBreak = "normal";
    textarea.wrap = "off";
  }

  updateLineNumbers(); // recalc after toggle
}

wrapButton.addEventListener("click", toggleWrap);

