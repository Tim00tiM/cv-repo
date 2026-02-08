import { configureStore } from "@reduxjs/toolkit";
import { getImageSlice } from "./ImageSlice";
import { getCommentsSlice } from "./CommentsSlice";
import { patternSlice } from "./CreatePatternSlice";
import { getUserSlice } from "./UserSlice";

export const store = configureStore({
  reducer: {
    images: getImageSlice.reducer,
    pattern: patternSlice.reducer,
    comments: getCommentsSlice.reducer,
    user: getUserSlice.reducer,
  },
});

// Infer the `RootState`,  `AppDispatch`, and `AppStore` types from the store itself
export type RootState = ReturnType<typeof store.getState>;
// Inferred type: {posts: PostsState, comments: CommentsState, users: UsersState}
export type AppDispatch = typeof store.dispatch;
export type AppStore = typeof store;
