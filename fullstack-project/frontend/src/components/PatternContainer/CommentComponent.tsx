import { Link } from "react-router-dom";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import { mockLoadComments } from "../../store/CommentsSlice";

import "../styles/text.css";
import "./CommentComponent.css";

import React from "react";

export default function CommentComponent(props) {
  const { author, id, text } = props;
  console.log(author);

  return (
    <div className="cococomment">
      <Link to={`/author/${id}`}>
        <div className="text author">{author}:</div>
        <div className="text comment-container">{text}</div>
      </Link>
    </div>
  );
}
