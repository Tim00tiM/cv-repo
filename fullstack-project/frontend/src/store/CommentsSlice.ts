import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { IImage, IComment, IUser } from "../model/model";
import { AppDispatch } from "./store";

interface ICommentsState {
  comments: IComment[];
}

const initialState = {
  comments: [],
} satisfies ICommentsState;

export const getCommentsSlice = createSlice({
  name: "comments",
  initialState,
  reducers: {
    setComments: (state: ICommentsState, action: PayloadAction<IComment[]>) => {
      state.comments = action.payload;
    },
    addCommentMock: (state: ICommentsState, action: PayloadAction<string>) => {
      let mockComment: IComment = {
        author_id: 1,
        text: action.payload,
        author_name: "you",
      };
      state.comments.push(mockComment);
      alert("success");
    },
    addCommentInner: (
      state: ICommentsState,
      action: PayloadAction<IComment>
    ) => {
      state.comments.push(action.payload);
    },
  },
});

const { setComments, addCommentMock, addCommentInner } =
  getCommentsSlice.actions;

const mockLoadComments = (id: number) => async (dispatch: AppDispatch) => {
  const response = await fetch("/mock_comments.json");
  console.log(response);
  const comments: IComment[] = await response.json();

  dispatch(setComments(comments));
};

const loadComments = (id: number) => async (dispatch: AppDispatch) => {
  // докинуть авторизации : UPD: какой авторизации, ты что, дурной.
  const response = await fetch(`/api/patterns/${id}/comments`);
  const comments: IComment[] = await response.json();

  dispatch(setComments(comments));
};

const addComment = (comment: IComment) => async (dispatch: AppDispatch) => {
  dispatch(addCommentInner(comment));
};

// const initComments = () => async (dispatch: AppDispatch) => {
//   dispatch()
// }

export { mockLoadComments, loadComments, addComment };
