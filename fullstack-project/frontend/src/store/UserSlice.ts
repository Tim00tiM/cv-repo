import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { IUser } from "../model/model";
import { AppDispatch } from "./store";
import { useAppSelector } from "../hooks/hooks";
import {
  getUserCookie,
  removeUserCookie,
  setUserCookie,
} from "../util/cookies";
import {
  compareTwoDates,
  getDateInString,
  getTodayDDMMYYYY,
} from "../util/Date";

interface IUserState {
  user: IUser;
}

const baseUser = {
  token: "",
  self_id: 0,
  expire_date: getDateInString(new Date("1970-01-01")),
  login: "",
};

const initialState = {
  user: getUserCookie() || baseUser,
} satisfies IUserState;

export const getUserSlice = createSlice({
  name: "user",
  initialState,
  reducers: {
    setTokenInner: (state: IUserState, action: PayloadAction<IUser>) => {
      console.log(action.payload);
      state.user = action.payload;
      setUserCookie(action.payload);
    },
    clearUser: (state: IUserState) => {
      state.user = baseUser;
      removeUserCookie(); // remove cookie
    },
  },
});

const { setTokenInner, clearUser } = getUserSlice.actions;

const setToken = (user: IUser) => async (dispatch: AppDispatch) => {
  dispatch(setTokenInner(user));
};

const checkToken = (user: IUser) => async (dispatch: AppDispatch) => {
  if (
    compareTwoDates(user.expire_date, getTodayDDMMYYYY()) &&
    user.token != ""
  ) {
    alert("Перезайдите в аккаунт");
    dispatch(clearUser());
  }
};

const clearToken = () => async (dispatch: AppDispatch) => {
  dispatch(clearUser());
};

export { setToken, checkToken, clearToken };
