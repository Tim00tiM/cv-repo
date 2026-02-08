import "./ColorDiag.css";
import "../styles/text.css";

import React from "react";
import { Link } from "react-router-dom";

import ColorPicker from "./ColorPicker";

export default function ColorDiag(props) {
  const { colors, removeColor, updateColor } = props;
  let pickers = [];
  for (let i = 0; i < colors.length; ++i) {
    pickers.push(
      <ColorPicker
        key={i}
        color={colors[i]}
        updateColor={(color) => updateColor(i, color)}
        removeColor={() => removeColor(i)}
      />
    );
  }
  return (
    <div className="colors-container">
      {pickers}
      <button
        onClick={(e) =>
          updateColor(colors.length, {
            red: 255,
            green: 255,
            blue: 255,
          })
        }
        className="new-color-button"
      >+</button>
    </div>
  );
}
