import { Link, useNavigate } from "react-router-dom";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import { mockLoadComments } from "../../store/CommentsSlice";

import { useState } from "react";

import "../styles/text.css";
import "./RegistrationMenu.css";

import React from "react";
import { registerUser } from "../../api/User";
import { AppDispatch } from "../../store/store";

async function register(
  login: string,
  password: string,
  email: string,
  dispatch: AppDispatch,
  navigate
) {
  if (login == "" || password == "" || email == "") {
    alert("Есть пустое поле");
    return;
  }
  let result = await registerUser(login, password, email, dispatch);
  if (result) {
    navigate("/");
  }
}

export default function RegistrationMenu(props) {
  const navigate = useNavigate();
  const user = useAppSelector((rootState) => rootState.user);
  const [email, setEmail] = useState("");
  const [login, setLogin] = useState("");
  const [password, setPassword] = useState("");
  const dispatch = useAppDispatch();
  return (
    <div className="menu-holder heading-2">
      <div>Регистрация</div>
      <input
        placeholder="Почта"
        value={email}
        className="cred-input"
        type="email"
        onChange={(e) => setEmail(e.target.value)}
      />
      <input
        placeholder="Логин"
        value={login}
        className="cred-input"
        type="text"
        onChange={(e) => setLogin(e.target.value)}
      />
      <input
        placeholder="Пароль"
        value={password}
        className="cred-input"
        type="password"
        onChange={(e) => setPassword(e.target.value)}
      />
      <button
        onClick={() => register(login, password, email, dispatch, navigate)}
        className="text"
      >
        Зарегистрироваться
      </button>
      <Link className="text auth" to="/auth">
        Есть аккаунт? Вход
      </Link>
    </div>
  );
}
