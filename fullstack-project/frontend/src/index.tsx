import * as React from "react";
import { createRoot } from "react-dom/client";
import { Provider } from "react-redux";
import { BrowserRouter, Routes, Route } from "react-router";

import Home from "./Pages/Home";
import Create from "./Pages/Create";
import Gallery from "./Pages/Gallery";
import { store } from "./store/store";
import Me from "./Pages/Me";
import Auth from "./Pages/Auth";
import Reg from "./Pages/Reg";
import PatternPage from "./Pages/PatternPage";
import RequireAuth from "./util/RequireAuth";

import "./index.css";

const root = createRoot(document.getElementById("root"));
root.render(
  <Provider
    store={store}
    children={
      <BrowserRouter>
        <Routes>
          <Route path="/" element={<Home />} />
          <Route
            path="/create"
            element={
              <RequireAuth children={<Create />} />
              // <Create />
            }
          />
          <Route path="/gallery" element={<Gallery />} />
          <Route path="/author/:id" element={<Me />} />
          <Route path="/auth" element={<Auth />} />
          <Route path="/reg" element={<Reg />} />
          <Route path="/pattern/:id" element={<PatternPage />} />
        </Routes>
      </BrowserRouter>
    }
  />
);
