import "./ColorPicker.css";
import "../styles/text.css";

import React from "react";
import { Link } from "react-router-dom";
import { hexToRgb } from "../../util/ColorCast";

export default function ColorPicker(props) {
  const { removeColor, updateColor, color } = props;
  let cssColor = `rgb(${color.red}, ${color.green}, ${color.blue})`;

  return (
    <div className="color-picker-item">
      <input
        type="color"
        value={cssColor}
        onChange={(e) => {
          console.log(e.target.value);
          updateColor(hexToRgb(e.target.value));
        }}
        className="color-input"
      />
      <button onClick={() => removeColor()} className="cross-button">
        ×
      </button>
    </div>
  );
}
