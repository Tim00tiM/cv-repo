export interface IImage {
  author_name?: string;
  author_id: number;
  diag: IPattern;
  creation_date: string;
  id?: string;
  title: string;
}

export interface IPattern {
  diags: IDiag[];
  width: number;
  height: number;
}

export interface IColor {
  red: number;
  green: number;
  blue: number;
}

export interface IDiag {
  colors: IColor[];
}

export interface IComment {
  author_id: number;
  text: string;
  author_name?: string;
}

export interface IUser {
  token: string;
  expire_date: string;
  self_id: number;
  login: string;
}
