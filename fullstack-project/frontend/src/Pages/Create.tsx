import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";

import "./Create.css";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import { mockLoadImages } from "../store/ImageSlice";
import { IImage } from "../model/model";
import PatternRender from "../components/PreviewCard/PatternRender";
import Creation from "../components/Creation/Creation";

export default function Create() {
  return (
    <>
      <div className="create-page-container">
        <Header />
        <Creation />
      </div>
      <Footer />
    </>
  );
}
