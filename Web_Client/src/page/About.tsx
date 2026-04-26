import { useState, useEffect } from "react";
import Footer from "../ui/footer";
import Header from "../ui/header";

function About() {
  const [contactEmail, setContactEmail] = useState("");

  useEffect(() => {
    // Fetch contact email from the server
    const fetchContactEmail = async () => {
      const res = await fetch("/api/email");
      const data = await res.text();
      setContactEmail(data);
    };

    fetchContactEmail();
  }, []);

  return (
    <>
      <Header />

      <div className="flex flex-col bg-[#292929] w-full items-center justify-center">      
        <div className="max-w-3xl mx-auto px-6 pt-16 flex flex-col items-center w-1/2">
          <a href="/" title="Home" className="cursor-default mb-6 block">
            <img src="/www/favicon.svg" alt="SharePaste Logo" className="w-24 h-24 sm:w-32 sm:h-32 object-contain cursor-default transition-transform hover:scale-105"></img>
          </a>

          <h1 className="text-4xl sm:text-5xl font-extrabold text-gray-900 dark:text-white tracking-tight mb-4">
            SharePaste
          </h1>

          <p className="text-lg sm:text-xl text-gray-600 dark:text-gray-300 font-medium text-center mb-10 max-w-2xl">
              A fast and lightweight paste service for sharing text and code.
          </p>
        </div>

        <div className="space-y-6 text-gray-700 dark:text-gray-300 text-base text-center sm:text-lg leading-relaxed w-1/2">
          <p>
              SharePaste is a simple pastebin-style service built for speed, readability,
              and ease of sharing. Just paste and share your text!
          </p>
        </div>

        <div className="bg-gray-50 dark:bg-gray-800/50 border border-gray-200 dark:border-gray-700 rounded-xl p-6 sm:p-8 space-y-4 my-8 shadow-sm text-gray-300 w-1/2">

          <h2 className="text-2xl font-bold text-gray-900 dark:text-white mb-2 border-b border-gray-200 dark:border-gray-700 pb-3">Reporting & Terms</h2>
          <p>
              Content hosted on SharePaste is user-generated. Illegal content, abuse,
              or copyright violations are not permitted or encouraged.
          </p>
          <ul className="space-y-3">
            <li>
                <b className="font-semibold text-gray-900 dark:text-white">1. No Illegal Use: </b>You may not use SharePaste to share, store, or distribute any content that is illegal, harmful, or violates any laws or regulations.
            </li>
            <li>
                <b className="font-semibold text-gray-900 dark:text-white">2. No Malicious Content: </b>Do not upload or share content intended to harm others, including malware, phishing links, or personal data without consent.
            </li>
            <li>
                <b className="font-semibold text-gray-900 dark:text-white">3. Content Responsibility: </b>You are solely responsible for the content you post. SharePaste does not pre-screen content and is not liable for what users choose to share.
            </li>
            <li>
                <b className="font-semibold text-gray-900 dark:text-white">4. Moderation: </b>We reserve the right to remove any content at our discretion and to restrict or terminate access for abuse or violations of these terms.
            </li>
            <li>
                <b className="font-semibold text-gray-900 dark:text-white">5. No Warranties or Liability: </b>SharePaste is provided “as is” with no guarantees. We do not guarantee uptime, data retention, privacy, or availability.
            </li>
          </ul>
          <p className="pt-4 border-t border-gray-200 dark:border-gray-700 mt-4">
              By using SharePaste, you accept these terms. If you do not agree, please do not use the service.
          </p>
          
          <p className="text-red-600 dark:text-red-400 font-medium">
              If you encounter content that violates the Terms of Service or applicable laws,
              please report it! Please contact the site administrator.
          </p>
        </div>
        
          <div className="w-1/2 mb-16">
            <p className="text-center text-sm sm:text-base text-white">
              This SharePaste instance is self-hosted. Please contact the server administrator for support or to report content.
              <b id="ContactEmail" className="font-semibold text-blue-600 dark:text-blue-400 hover:text-blue-800 dark:hover:text-blue-300 ml-1">
                {contactEmail ? (
                  <a href={`mailto:${contactEmail}`} className="hover:underline">
                    {contactEmail}
                  </a>
                ) : (
                  "None provided."
                )}
              </b>
            </p>
          </div>

        </div>

      <Footer />
    </>
  );
}

export default About;