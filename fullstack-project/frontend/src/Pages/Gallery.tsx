import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";
import "./Gallery.css";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import { mockLoadImages } from "../store/ImageSlice";
import DiagPreviews from "../components/DiagPreviews/DiagPreviews";
import CreateDiag from "../components/CreateDiagButton/CreateDiag";

import FirstVisitHandler from "../util/LoadImagesHandler";

export default function Gallery() {
  FirstVisitHandler();
  const images = useAppSelector((rootState) => rootState.images);
  console.log(images);
  return (
    <>
      <div className="root-style">
        <Header />
        <div className="diag-previews-container">
          <DiagPreviews images={images.images} />
        </div>
      </div>
      <CreateDiag />

      <Footer />
    </>
  );
}
