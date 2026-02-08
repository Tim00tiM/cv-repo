import { Link, useNavigate } from "react-router-dom";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import { mockLoadComments } from "../../store/CommentsSlice";

import { useState } from "react";

import "../styles/text.css";
import "./AuthMenu.css";

import React from "react";
import { AppDispatch } from "../../store/store";
import { loginUser } from "../../api/User";

async function loginFun(
  email: string,
  password: string,
  dispatch: AppDispatch,
  navigate
) {
  if (email == "" || password == "") {
    alert("Есть пустое поле");
    return;
  }
  let result = await loginUser(email, password, dispatch);
  if (result) {
    navigate("/");
  }
}

export default function AuthMenu(props) {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const navigate = useNavigate();
  const dispatch = useAppDispatch();
  return (
    <div className="menu-holder heading-2">
      <div>Вход</div>
      <input
        placeholder="Почта"
        value={email}
        className="cred-input"
        type="text"
        onChange={(e) => setEmail(e.target.value)}
      />
      <input
        placeholder="Пароль"
        value={password}
        className="cred-input"
        type="password"
        onChange={(e) => setPassword(e.target.value)}
      />
      <button
        onClick={() => loginFun(email, password, dispatch, navigate)}
        className="text"
      >
        Войти
      </button>
    </div>
  );
}
