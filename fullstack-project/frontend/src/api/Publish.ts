import { IComment, IImage, IPattern, IUser } from "../model/model";
import { addComment } from "../store/CommentsSlice";
import { addImages } from "../store/ImageSlice";
import { AppDispatch } from "../store/store";
import { getTodayDDMMYYYY } from "../util/Date";

const randomInt = (min, max) =>
  Math.floor(Math.random() * (max - min + 1)) + min;

const publishPattern = async (
  dispatch: AppDispatch,
  title: string,
  pattern: IPattern,
  user: IUser
) => {
  try {
    const body: IImage = {
      author_id: user.self_id,
      creation_date: getTodayDDMMYYYY(),
      title: title,
      diag: pattern,
    };
    const response = await fetch("/api/patterns", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${user.token}`,
      },
      body: JSON.stringify(body),
    });
    if (response.status > 205 || response.status < 200) {
      alert(await response.text());
    } else {
      const data = await response.json();
      console.log("Success:", data);
      dispatch(addImages([body]));
      return true;
    }
  } catch (error) {
    console.error("Error:", error);
  }
  return false;
};

const publishComment = async (
  dispatch: AppDispatch,
  user: IUser,
  pattern_id: string,
  text: string
) => {
  try {
    const body: IComment = {
      author_id: user.self_id,
      text: text,
      author_name: user.login,
    };
    const response = await fetch(`/api/patterns/${pattern_id}/comments`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${user.token}`,
      },
      body: JSON.stringify(body),
    });
    if (response.status > 205 || response.status < 200) {
      alert(await response.text());
    } else {
      const data = await response.text();
      console.log("Success:", body);
      dispatch(addComment(body));
      return true;
    }
  } catch (error) {
    console.error("Error:", error);
  }
  return false;
};

const publishMock = async (dispatch, title, pattern) => {
  const id = randomInt(150, 55555);
  const body: IImage = {
    author_name: "john james",
    author_id: 1,
    id: id,
    creation_date: getTodayDDMMYYYY(),
    title: title,
    diag: pattern,
  };
  dispatch(addImages([body]));
  alert(`Опубликовано с id ${id}`);
};

export { publishPattern, publishMock, publishComment };
