import React, { useRef, useEffect } from "react";
import { IPattern, IColor, IImage } from "../../model/model";
import { createPath } from "react-router-dom";

function renderCanvas(
  canvasRef,
  pattern: IPattern,
  pixelSize: number,
  targetSize: number
) {
  const canvas = canvasRef.current;
  if (!canvas) return;

  const ctx = canvas.getContext("2d");
  if (!ctx) return;

  const originalWidth = pattern.width * pixelSize;
  const originalHeight = pattern.height * pixelSize;

  const scale = Math.min(
    targetSize / originalWidth,
    targetSize / originalHeight
  );

  canvas.width = originalWidth;
  canvas.height = originalHeight;
  // const offsetX = (targetSize - originalWidth * scale) / 2;
  // const offsetY = (targetSize - originalHeight * scale) / 2;

  ctx.clearRect(0, 0, targetSize, targetSize);

  // ctx.save();
  // ctx.translate(offsetX, offsetY);

  // ctx.scale(scale, scale);
  for (
    let i = 1;
    i <= Math.min(pattern.diags.length, pattern.width + pattern.height - 1);
    ++i
  ) {
    let widthReducer = Math.max(0, i - pattern.width);
    let heightReducer = Math.max(0, i - pattern.height);
    for (let j = 0; j < i - widthReducer - heightReducer; ++j) {
      const x = heightReducer + j;
      const y = i - 1 - j - heightReducer;
      let color: IColor = {
        red: 255,
        green: 255,
        blue: 255,
      };
      if (pattern.diags[i - 1].colors.length != 0) {
        color =
          pattern.diags[i - 1].colors[j % pattern.diags[i - 1].colors.length];
      }
      ctx.fillStyle = `rgb(${color.red}, ${color.green}, ${color.blue})`;
      ctx.fillRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
    }
  }

  // ctx.restore();
}

export default function PatternRender(props) {
  const {
    pattern,
    targetSize,
    style,
  }: { pattern: IPattern; targetSize: number; style: object } = props;
  const pixelSize = 30;
  const canvasRef = useRef<HTMLCanvasElement>(null);

  const originalWidth = pattern.width * pixelSize;
  const originalHeight = pattern.height * pixelSize;

  const scale = Math.min(
    targetSize / originalWidth,
    targetSize / originalHeight
  );

  useEffect(() => {
    renderCanvas(canvasRef, pattern, pixelSize, targetSize);
  }, [pattern]);

  return (
    <canvas
      ref={canvasRef}
      style={{
        width: originalWidth * scale,
        height: originalHeight * scale,
        display: "block",
        borderStyle: "solid",
        borderWidth: "2px",
        borderColor: "black",
        ...style,
      }}
    />
  );
}
