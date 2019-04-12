import { firestore } from 'firebase';

export enum ApplicantStatus {
  UNKNOWN = 0,
  CORFIRMED,
  REJECTED
}

export enum JobStatus {
  PUBLISHED = 0,
  CONNECTED,
  DONE
}

export interface Job {
  id?: string;
  name: string;
  description: string;
  category?: string;
  dueDate: firestore.Timestamp;
  icon: string;
  userID: string;
  username?: string;
  address: string;
  location: firestore.GeoPoint;
  applicants: Applicant[];
  status: JobStatus;
}

export interface Applicant {
  userId: string;
  status: ApplicantStatus;
}

export enum JobRelation {
  UNKNOWN = 0,
  APPLIED,
  CREATED
}
