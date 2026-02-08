import { IUser } from "../model/model";
import { removeImage } from "../store/ImageSlice";
import { AppDispatch } from "../store/store";

const deletePattern = async (
  dispatch: AppDispatch,
  user: IUser,
  pattern_id: string
) => {
  try {
    const response = await fetch(`/api/patterns/${pattern_id}`, {
      method: "DELETE",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${user.token}`,
      },
    });

    if (!response.ok) {
      const errorText = await response.text();
      alert(`Failed to delete pattern: ${errorText}`);
      return false;
    }

    dispatch(removeImage(pattern_id));

    console.log(`Pattern ${pattern_id} deleted successfully`);
    return true;
  } catch (error) {
    console.error("Error deleting pattern:", error);
  }
  return false;
};
export { deletePattern };
