import { Pipe, PipeTransform } from '@angular/core';
import { Observable, ReplaySubject } from 'rxjs';
import { filter, tap } from 'rxjs/operators';

@Pipe({
  name: 'imagePipe'
})
export class ImagePipe implements PipeTransform {

  constructor() {
  }

  public transform(value: any): Observable<string | ArrayBuffer> {
    const reader = new FileReader();
    const obs = new ReplaySubject<string | ArrayBuffer>();
    reader.readAsDataURL(value);
    reader.onload = () => obs.next(reader.result); // tslint:disable-line
    return obs.asObservable().pipe(filter(item => item !== ''));
  }
}
