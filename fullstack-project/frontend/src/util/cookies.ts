import Cookies from "js-cookie";
import { IUser } from "../model/model";

export const setUserCookie = (user: IUser) => {
  Cookies.set("user", JSON.stringify(user), { expires: 1 });
};

export const getUserCookie = (): IUser | null => {
  const cookie = Cookies.get("user");
  return cookie ? JSON.parse(cookie) : null;
};

export const removeUserCookie = () => {
  Cookies.remove("user");
};
