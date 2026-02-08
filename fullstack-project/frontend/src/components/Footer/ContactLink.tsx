import "../styles/text.css";

import "./ContactLink.css";

import React from "react";
import { IContactLink } from "./IContactLink";

export default function ContactLink(props: IContactLink) {
  return (
    <a className="text contact-link" href={props.link}>
      {props.name}
    </a>
  );
}
