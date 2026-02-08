import { Link } from "react-router-dom";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import { loadComments } from "../../store/CommentsSlice";
import CommentComponent from "./CommentComponent";
import "./PatternContainer.css";

import { publishComment } from "../../api/Publish";

import React from "react";
import { useEffect, useState } from "react";
import { checkToken } from "../../store/UserSlice";

export default function PatternContainer(props) {
  const { preview, id, title, author_name, author_id } = props;

  const comments = useAppSelector((rootState) => rootState.comments);
  const user = useAppSelector((rootState) => rootState.user);
  const dispatch = useAppDispatch();

  useEffect(() => {
    dispatch(loadComments(id));
  }, []);

  const [commentText, setCommentText] = useState("");

  let commentComponents = [];

  for (let i = 0; i < comments.comments.length; ++i) {
    commentComponents.push(
      <CommentComponent
        author={comments.comments[i].author_name}
        text={comments.comments[i].text}
        id={comments.comments[i].author_id}
      />
    );
  }

  return (
    <div>
      <div className="pattern-page">
        <div className="pattern-title">
          <div className="text">{title}</div>
          <Link className="text" to={`/author/${author_id}`}>
            By: {author_name}
          </Link>
          <div className="pattern-preview">{preview}</div>
        </div>

        <div className="comments-section">
          <div className="comments">{commentComponents}</div>
          <div className="new-comment">
            <input
              className="text input-field"
              type="text"
              value={commentText}
              onChange={(e) => setCommentText(e.target.value)}
              placeholder="Введите текст комментария"
            />
            <button
              onClick={() => {
                if (user.user.token == "") {
                  alert("Войдите в аккаунт");
                } else {
                  publishComment(dispatch, user.user, id, commentText);
                }
              }}
              className="publish-button-comment text"
            >
              Опубликовать
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}
