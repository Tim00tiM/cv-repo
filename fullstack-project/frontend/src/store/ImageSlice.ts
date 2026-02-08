import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { IImage } from "../model/model";
import { AppDispatch } from "./store";

interface IImagesState {
  images: Record<number, IImage>;
}

const initialState = {
  images: {},
} satisfies IImagesState;

export const getImageSlice = createSlice({
  name: "images",
  initialState,
  reducers: {
    addImagesInner: (state: IImagesState, action: PayloadAction<IImage[]>) => {
      console.log(action);
      // if (action == null || action.payload == null) {
      // return;
      // }
      action.payload.forEach((image) => (state.images[image.id] = image));
    },
    removeImageInner: (
      state: IImagesState,
      action: PayloadAction<string | string[]>
    ) => {
      if (Array.isArray(action.payload)) {
        action.payload.forEach((id) => delete state.images[id]);
      } else {
        delete state.images[action.payload];
      }
    },
  },
});

const { addImagesInner, removeImageInner } = getImageSlice.actions;

const mockLoadImages = () => async (dispatch: AppDispatch) => {
  const response = await fetch("/mock_diag.json");
  const images: IImage[] = await response.json();

  dispatch(addImagesInner(images));
};

const loadImagesHome = () => async (dispatch: AppDispatch) => {
  const response = await fetch("/api/patterns");
  const images: IImage[] = await response.json();

  dispatch(addImagesInner(images));
};

const loadImagesById = (id: string) => async (dispatch: AppDispatch) => {
  const response = await fetch(`/api/patterns/user/${id}`);
  const images: IImage[] = await response.json();

  dispatch(addImagesInner(images));
};

const addImages = (images: IImage[]) => async (dispatch: AppDispatch) => {
  dispatch(addImagesInner(images));
};

const removeImage =
  (imageIds: string | string[]) => async (dispatch: AppDispatch) => {
    dispatch(removeImageInner(imageIds));
  };

export {
  mockLoadImages,
  loadImagesHome,
  loadImagesById,
  addImages,
  removeImage,
};
