import "./Footer.css";
import ContactLink from "./ContactLink";
import "../styles/text.css";

import React from "react";
import { Link } from "react-router-dom";

export default function Footer() {
  return (
    <footer className="footer-container">
      <ContactLink name="GitHub(тут заглушка)" link="https://github.com" />
      <ContactLink name="TG(тут заглушка)" link="https://t.me/" />
    </footer>
  );
}
