import "./Publish.css";
import "../styles/text.css";

import React, { useEffect } from "react";
import { Link, useNavigate } from "react-router-dom";
import { IImage } from "../../model/model";
import { addImages } from "../../store/ImageSlice";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import { publishPattern } from "../../api/Publish";
import { RootState } from "../../store/store";
import { checkToken } from "../../store/UserSlice";

export default function Publish(props) {
  const { title, pattern } = props;
  const dispatch = useAppDispatch();
  const user = useAppSelector((rootState) => rootState.user);
  dispatch(checkToken(user.user));
  const navigate = useNavigate();

  useEffect(() => {
    dispatch(checkToken(user.user));
  }, [dispatch]);

  useEffect(() => {
    if (!user.user.token) {
      navigate("/reg");
    }
  }, [user.user.token, navigate]);
  console.log(`user: ${user.user.token}`);

  return (
    <button
      className="publish-button text"
      onClick={(e) => {
        publishPattern(dispatch, title, pattern, user.user);
        navigate("/");
      }}
    >
      Опубликовать
    </button>
  );
}
