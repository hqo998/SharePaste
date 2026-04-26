import Footer from "../ui/footer";
import Header from "../ui/header";

function Home() {
  return (
    <div className="flex flex-col h-screen">
      <Header />

      
      <div className="flex flex-1 w-full">
        <div id="lineNumbers" className="bg-[#292929] text-[#b9b9b9] text-right select-none overflow-hidden font-bold whitespace-pre font-mono text-[16px] p-2.5 leading-[1.4em]">1</div>
        <textarea
          id="pasteBox"
          name="pasteText"
          className="bg-[#1f1f22] text-[#d6d6d6] w-full flex-1 resize-none whitespace-pre-wrap wrap-break-word focus:outline-none font-mono text-[16px] p-2.5 leading-[1.4em] overflow-y-hidden overflow-x-auto"
          spellCheck={false}
          maxLength={5000000}
        ></textarea>
      </div>

      <Footer />
    </div>
  );
}

export default Home;