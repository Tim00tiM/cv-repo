import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";

import "./Reg.css";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import { mockLoadImages } from "../store/ImageSlice";
import RegistrationMenu from "../components/RegistrationMenu/RegistrationMenu";

export default function Reg() {
  return (
    <>
      <div className="reg-page-container">
        <Header />
        <RegistrationMenu />
      </div>
      <Footer />
    </>
  );
}
