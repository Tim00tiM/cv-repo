import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";

import "./Auth.css";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import { mockLoadImages } from "../store/ImageSlice";

import "../components/styles/text.css";
import AuthMenu from "../components/AuthMenu/AuthMenu";

export default function Auth() {
  return (
    <>
      <div className="auth-page-container">
        <Header />
        <AuthMenu />
      </div>
      <Footer />
    </>
  );
}
