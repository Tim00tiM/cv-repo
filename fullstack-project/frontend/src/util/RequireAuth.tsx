import { useNavigate } from "react-router-dom";
import { useAppSelector } from "../hooks/hooks";
import { useEffect } from "react";

export default function RequireAuth({ children }) {
  const token = useAppSelector((state) => state.user.user.token);
  const navigate = useNavigate();

  useEffect(() => {
    if (!token) {
      navigate("/reg");
    }
  }, [token, navigate]);

  if (!token) return null;

  return children;
}
