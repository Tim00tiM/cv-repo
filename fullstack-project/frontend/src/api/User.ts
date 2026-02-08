import { AppDispatch } from "../store/store";
import { setToken } from "../store/UserSlice";
import { IUser } from "../model/model";
import { jwtDecode } from "jwt-decode";
import { getDateInString } from "../util/Date";

interface JwtPayload {
  exp: number;
  iat: number;
  user_id?: number;
}

const registerUser = async (
  login: string,
  password: string,
  email: string,
  dispatch: AppDispatch
): Promise<boolean> => {
  let user: IUser = {
    token: "3",
    self_id: 1,
    expire_date: "12.12.2026",
    login: "loh",
  };
  // dispatch(setToken(user));
  // return true;
  const response = await fetch("/api/register", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({
      login: login,
      password: password,
      email: email,
    }),
  });
  if (response.status > 205 || response.status < 200) {
    alert(await response.text());
  } else {
    try {
      const user: IUser = await response.json();
      const payload: JwtPayload = jwtDecode(user.token);
      const expDate = new Date(payload.exp * 1000);
      user.expire_date = getDateInString(expDate);
      dispatch(setToken(user));
      return true;
    } catch (e) {
      console.log(e);
    }
  }
  return false;
};

const loginUser = async (
  email: string,
  password: string,
  dispatch: AppDispatch
): Promise<boolean> => {
  const response = await fetch("/api/login", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ email: email, password: password }),
  });
  if (response.status > 205 || response.status < 200) {
    alert(await response.text());
  } else {
    try {
      const user: IUser = await response.json();
      const payload: JwtPayload = jwtDecode(user.token);
      const expDate = new Date(payload.exp * 1000);
      user.expire_date = getDateInString(expDate);
      dispatch(setToken(user));
      return true;
    } catch (e) {
      console.log(e);
    }
  }
  return false;
};

export { registerUser, loginUser };
