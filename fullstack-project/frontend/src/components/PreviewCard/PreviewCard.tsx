import "./PreviewCard.css";
import "../styles/text.css";

import React from "react";
import DiagExample from "../../images/example_diag.png";
import PatternRender from "./PatternRender";
import { Link } from "react-router";
import { IImage } from "../../model/model";
import { deletePattern } from "../../api/Delete";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";

export default function PreviewCard(props) {
  const { image, canDelete }: { image: IImage; canDelete: boolean } = props;
  const dispatch = useAppDispatch();
  const user = useAppSelector((rootState) => rootState.user);

  return (
    <div className="diag-full-container">
      <Link className="card" to={`/pattern/${image.id}`}>
        <div className="text force-title">
          {image.title.length > 0 ? image.title : " "}
        </div>
        <PatternRender pattern={image.diag} targetSize={150} />
        <div className="text">{image.author_name}</div>
        <div className="text">{image.creation_date}</div>
      </Link>
      {canDelete ? (
        <button
          onClick={() => deletePattern(dispatch, user.user, image.id)}
          className="cross-button-pattern"
        >
          ×
        </button>
      ) : null}
    </div>
  );
}
