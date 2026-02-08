import "./Header.css";
import "../styles/text.css";

import React from "react";
import { Link } from "react-router-dom";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import { clearToken } from "../../store/UserSlice";

export default function Header() {
  const user = useAppSelector((rootState) => rootState.user);
  const dispatch = useAppDispatch();
  return (
    <header className="header-container">
      <Link className="logo heading-2" to="/">
        DIAG
      </Link>
      <div className="nav">
        <Link className="heading-1" to="/gallery">
          Галерея
        </Link>
      </div>
      <div className="user-icon">
        <Link
          to={user.user.token == "" ? "/reg" : "/author/" + user.user.self_id}
        >
          <div
            className="circle"
            style={{
              backgroundColor: user.user.token == "" ? "#e0e0e0" : "#109210ff",
            }}
          ></div>
        </Link>
        {user.user.token ? (
          <button
            onClick={() => dispatch(clearToken())}
            className="cross-button-login"
          >
            ×
          </button>
        ) : null}
      </div>
    </header>
  );
}
