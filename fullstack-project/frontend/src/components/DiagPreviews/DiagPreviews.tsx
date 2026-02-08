import "./DiagPreviews.css";
import "../styles/text.css";

import React from "react";
import PreviewCard from "../PreviewCard/PreviewCard";
import { IImage } from "../../model/model";

export default function DiagPreviews(props) {
  const { images, canDelete }: { images: IImage[]; canDelete: boolean } = props;

  var imagePreviews = [];

  for (const [key, value] of Object.entries(images)) {
    console.log(key, value);
    imagePreviews.push(<PreviewCard image={value} canDelete={canDelete} />);
  }

  return <div className="card-grid">{imagePreviews}</div>;
}
