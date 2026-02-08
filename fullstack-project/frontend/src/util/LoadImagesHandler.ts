import { useEffect, useRef } from "react";
import {
  loadImagesById,
  loadImagesHome,
  mockLoadImages,
} from "../store/ImageSlice";
import { setWidth } from "../store/CreatePatternSlice";
import { useAppDispatch } from "../hooks/hooks";
import { setToken } from "../store/UserSlice";
import { getUserCookie } from "./cookies";

export default function LoadImagesHandler(id: string = "-1") {
  const dispatch = useAppDispatch();
  const firstVisitRef = useRef(true);

  const dispatchMockLoadImages = () => {
    dispatch(mockLoadImages());
  };

  const dispatchLoadImagesHome = () => {
    dispatch(loadImagesHome());
  };

  const dispatchLoadImagesById = () => {
    dispatch(loadImagesById(id));
  };

  useEffect(() => {
    if (firstVisitRef.current) {
      firstVisitRef.current = false;
      // dispatch(setToken(getUserCookie()));
      dispatch(setWidth(10));
      // dispatch(initComments())
    }
    if (id != "-1") {
      // download images by id
      dispatchLoadImagesById();
      // dispatchMockLoadImages();
    } else {
      dispatchLoadImagesHome();
      // dispatchMockLoadImages();
    }
  }, []);
  return null;
}
