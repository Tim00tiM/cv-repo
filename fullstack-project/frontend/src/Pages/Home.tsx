import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";

import "./Home.css";
// import "../index.css";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import CreateDiag from "../components/CreateDiagButton/CreateDiag";
import DiagPreviews from "../components/DiagPreviews/DiagPreviews";
import { mockLoadImages } from "../store/ImageSlice";
import FirstVisitHandler from "../util/LoadImagesHandler";

export default function Home() {
  FirstVisitHandler();
  const images = useAppSelector((rootState) => rootState.images);
  console.log(images);
  return (
    <>
      <div className="root-style">
        <Header />
        <div className="home-page-container">
          <div className="text">
            Вы попали на сайт для создания узоров Diag. Здесь вы сможете
            создавать свои узоры и делиться ими с другими пользователями сети
            Интернет
          </div>
          <DiagPreviews images={images.images} />
        </div>
      </div>

      <CreateDiag />
      <Footer />
    </>
  );
}
