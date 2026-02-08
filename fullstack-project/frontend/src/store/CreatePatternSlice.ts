// store/patternSlice.js
import { createSlice } from "@reduxjs/toolkit";
import { IDiag } from "../model/model";

interface IPatternState {
  width: number;
  height: number;
  diags: IDiag[];
}

const initialState = {
  width: 10,
  height: 10,
  diags: [],
} satisfies IPatternState;

function updateDiagonal(state: IPatternState) {
  const leng = state.diags.length
  for (
    let i = 0;
    i < (state.width + state.height + 1) - leng;
    ++i
  ) {
    state.diags.push({ colors: [] });
  }
}

export const patternSlice = createSlice({
  name: "pattern",
  initialState: initialState,
  reducers: {
    setWidth: (state, action) => {
      state.width = action.payload;
      updateDiagonal(state);
    },
    setHeight: (state, action) => {
      state.height = action.payload;
      updateDiagonal(state);
    },
    updateDiagonalColor: (state, action) => {
      const { diagIndex, colorIndex, color } = action.payload;
      if (colorIndex == state.diags[diagIndex].length) {
        state.diags[diagIndex].colors.push({});
      }
      state.diags[diagIndex].colors[colorIndex] = color;
    },
    removeDiagonalColor: (state, action) => {
      const { diagIndex, colorIndex } = action.payload;
      state.diags[diagIndex].colors.splice(colorIndex, 1);
    },
  },
});

export const {
  setWidth,
  setHeight,
  updateDiagonalColor,
  removeDiagonalColor,
} = patternSlice.actions;
