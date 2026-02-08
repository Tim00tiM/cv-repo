const getTodayDDMMYYYY = () => {
  const d = new Date();
  return `${String(d.getDate()).padStart(2, "0")}.${String(
    d.getMonth() + 1
  ).padStart(2, "0")}.${d.getFullYear()}`;
};

const getDateInString = (d: Date) => {
  return `${String(d.getDate()).padStart(2, "0")}.${String(
    d.getMonth() + 1
  ).padStart(2, "0")}.${d.getFullYear()}`;
};

const parseDDMMYYYY = (str: string): Date => {
  const [day, month, year] = str.split(".").map(Number);
  return new Date(year, month - 1, day);
};

const compareTwoDates = (first: string, second: string): boolean => {
  return parseDDMMYYYY(first) < parseDDMMYYYY(second);
};

export { getTodayDDMMYYYY, getDateInString, compareTwoDates };
