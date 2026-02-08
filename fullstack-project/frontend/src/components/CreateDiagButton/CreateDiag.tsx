import "../CreateDiagButton/CreateDiag.css";
import "../styles/text.css";

import React from "react";
import { Link } from "react-router";

export default function CreateDiag() {
  return (
    <Link to="/create">
      <button className="button-container text">Создать узор</button>
    </Link>
  );
}
