import "./Creation.css";
import PatternRender from "../PreviewCard/PatternRender";
import "../styles/text.css";

import React from "react";

import { Link } from "react-router";
import { useState, useEffect, useRef } from "react";
import { useAppDispatch, useAppSelector } from "../../hooks/hooks";
import {
  setWidth,
  setHeight,
  updateDiagonalColor,
  removeDiagonalColor,
} from "../../store/CreatePatternSlice";
import { IColor } from "../../model/model";
import ColorDiag from "./ColorDiag";
import Publish from "./Publish";
import LoadImagesHandler from "../../util/LoadImagesHandler";

export default function Creation() {
  LoadImagesHandler();
  const dispatch = useAppDispatch();
  const { width, height, diags } = useAppSelector(
    (rootState) => rootState.pattern
  );
  const [title, setTitle] = useState("");

  let preview = (
    <PatternRender
      pattern={{
        diags: diags,
        width: width,
        height: height,
      }}
      targetSize={600}
    />
  );

  const handleColorUpdate = (
    diagIndex: number,
    colorIndex: number,
    newColor: IColor
  ) => {
    dispatch(
      updateDiagonalColor({
        diagIndex: diagIndex,
        colorIndex: colorIndex,
        color: newColor,
      })
    );
  };

  const handleColorRemove = (diagIndex: number, colorIndex: number) => {
    dispatch(
      removeDiagonalColor({
        diagIndex: diagIndex,
        colorIndex: colorIndex,
      })
    );
  };

  return (
    <div className="creation-container">
      <div className="container-preview">
        <input
          type="text"
          value={title}
          onChange={(e) => setTitle(e.target.value)}
          placeholder="Введите название"
          className="text"
        />
        <div className="creation-preview">{preview}</div>
      </div>

      <div className="creation-menu-container">
        <div className="creation-menu">
          <div className="sizes-container">
            Размер: W:{" "}
            <input
              type="number"
              min={1}
              value={width}
              onChange={(e) => dispatch(setWidth(Number(e.target.value)))}
            />
            H:{" "}
            <input
              type="number"
              min={1}
              value={height}
              onChange={(e) => dispatch(setHeight(Number(e.target.value)))}
            />
          </div>
          {(() => {
            const elements = [];
            if (diags == undefined) {
              return elements;
            }
            for (
              let i = 0;
              i < Math.min(width + height - 1, diags.length);
              i++
            ) {
              ``;
              elements.push(
                <ColorDiag
                  key={i}
                  colors={diags[i].colors}
                  removeColor={(index) => handleColorRemove(i, index)}
                  updateColor={(index, color) =>
                    handleColorUpdate(i, index, color)
                  }
                />
              );
            }
            return elements;
          })()}
        </div>
        <Publish
          title={title}
          pattern={{
            width: width,
            height: height,
            diags: diags,
          }}
        />
      </div>
    </div>
  );
}
