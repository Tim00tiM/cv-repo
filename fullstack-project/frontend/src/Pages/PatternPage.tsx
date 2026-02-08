import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";
import { useParams } from "react-router";

import "./PatternPage.css";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import { addImages, mockLoadImages } from "../store/ImageSlice";
import PatternRender from "../components/PreviewCard/PatternRender";
import PatternContainer from "../components/PatternContainer/PatternContainer";
import { IImage } from "../model/model";
import { useEffect } from "react";

export default function PatternPage() {
  const { id } = useParams();
  const images: { images: Record<number, IImage> } = useAppSelector(
    (rootState) => rootState.images
  );

  const dispatch = useAppDispatch();
  const thisPattern: IImage = images.images[id];
  useEffect(() => {
    const fetchPattern = async () => {
      try {
        const resp = await fetch(`/api/patterns/${id}`, {
          headers: {
            "Content-Type": "application/json",
          },
        });

        if (!resp.ok) {
          throw new Error("Failed to fetch user");
        }

        const data = await resp.json();
        dispatch(addImages([data]));
      } catch (err) {
        console.error(err);
      }
    };

    fetchPattern();
  }, [id]);
  const preview =
    thisPattern != undefined ? (
      <PatternRender pattern={thisPattern.diag} targetSize={600} />
    ) : null;
  return (
    <>
      <div className="pattern-page-container">
        <Header />
        {thisPattern != undefined ? (
          <PatternContainer
            preview={preview}
            id={id}
            title={thisPattern.title}
            author_name={thisPattern.author_name}
            author_id={thisPattern.author_id}
          />
        ) : null}
      </div>
      <Footer />
    </>
  );
}
