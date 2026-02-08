import * as React from "react";
import { useAppDispatch, useAppSelector } from "../hooks/hooks";

import Header from "../components/Header/Header";
import Footer from "../components/Footer/Footer";
import { mockLoadImages } from "../store/ImageSlice";

import FirstVisitHandler from "../util/LoadImagesHandler";
import { useParams } from "react-router-dom";
import PreviewCard from "../components/PreviewCard/PreviewCard";
import { IImage, IUser } from "../model/model";
import DiagPreviews from "../components/DiagPreviews/DiagPreviews";

import "./Me.css";
import "../index.css";
import { useEffect, useState } from "react";
import { setToken } from "../store/UserSlice";

export default function Me() {
  const { id } = useParams();
  const dispatch = useAppDispatch();

  FirstVisitHandler(id);

  const images = useAppSelector((rootState) => rootState.images);
  const user = useAppSelector((rootState) => rootState.user);
  let pureImages: Record<number, IImage> = images.images;
  let yourImages: Record<number, IImage> = {};
  // console.log(images);
  let [authorName, setAuthorName] = useState("");
  let [isEditing, setIsEditing] = useState(false);
  let [newName, setNewName] = useState("");

  for (const [key, value] of Object.entries(pureImages)) {
    // console.log(key, value.author_id);
    if (value.author_id == Number(id)) {
      yourImages[key] = value;
    }
  }

  useEffect(() => {
    const fetchUser = async () => {
      try {
        const resp = await fetch(`/api/users/${id}`, {
          headers: {
            "Content-Type": "application/json",
          },
        });

        if (!resp.ok) {
          throw new Error("Failed to fetch user");
        }

        const data = await resp.json();
        setAuthorName(data.login);
      } catch (err) {
        console.error(err);
      }
    };

    fetchUser();
    // setAuthorName(user.user.login);
  }, [id]);

  const handleSubmit = async () => {
    try {
      const resp = await fetch(`/api/users/${id}`, {
        method: "PUT",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${user.user.token}`,
        },
        body: JSON.stringify({ login: newName }),
      });

      if (!resp.ok) {
        throw new Error("Failed to update name");
      }

      const data = await resp.json();
      setAuthorName(data.login);
      setIsEditing(false);

      const copy: IUser = {
        ...user.user,
        login: data.login,
      };
      dispatch(setToken(copy));
    } catch (err) {
      console.error(err);
      alert("Failed to update name");
    }
  };

  return (
    <>
      <div className="root-style">
        <Header />
        <div className="me-page-container">
          <div className="me-page-author-name heading-1">
            Author:{" "}
            {user.user.self_id === Number(id) && isEditing ? (
              <>
                <input
                  value={newName}
                  onChange={(e) => setNewName(e.target.value)}
                  className="author-name-input text"
                />
                <button
                  onClick={() => handleSubmit()}
                  className="author-name-button text"
                >
                  Save
                </button>
                <button
                  onClick={() => setIsEditing(false)}
                  className="author-name-button text"
                >
                  Cancel
                </button>
              </>
            ) : (
              <>
                {authorName}
                {user.user.self_id === Number(id) && (
                  <button
                    onClick={() => setIsEditing(true)}
                    className="author-name-button-edit text"
                  >
                    Edit
                  </button>
                )}
              </>
            )}
          </div>
          <DiagPreviews
            images={yourImages}
            canDelete={user.user.self_id == Number(id)}
          />
        </div>
      </div>
      <Footer />
    </>
  );
}
