const rgbToHex = (r, g, b) => {
    if (r === undefined || g === undefined || b === undefined) {
      return "#ffffffff";
    }
    return (
      "#" +
      [r, g, b]
        .map((x) => {
          const hex = x.toString(16);
          return hex.length === 1 ? "0" + hex : hex;
        })
        .join("")
    );
  };

  const hexToRgb = (hex) => {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result
      ? {
          red: parseInt(result[1], 16),
          green: parseInt(result[2], 16),
          blue: parseInt(result[3], 16),
        }
      : { red: 255, green: 255, blue: 255 };
  };

export { rgbToHex, hexToRgb} 